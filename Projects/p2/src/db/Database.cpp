//
// Created by liu on 18-10-23.
//

#include "Database.h"

#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../utils/formatter.h"
#include "../utils/uexception.h"
#include "Table.h"

std::unique_ptr<Database> Database::instance = nullptr;

void Database::testDuplicate(const std::string &tableName) {
  const std::unique_lock<std::shared_mutex> lock(mutex_);
  auto iterator = this->tables.find(tableName);
  if (iterator != this->tables.end()) {
    throw DuplicatedTableName("Error when inserting table \"" + tableName +
                              "\". Name already exists.");
  }
}

auto Database::registerTable(Table::Ptr &&table) -> Table & {
  const std::unique_lock<std::shared_mutex> lock(mutex_);
  auto name = table->name();
  auto iterator = this->tables.find(name);
  if (iterator != this->tables.end()) {
    throw DuplicatedTableName("Error when inserting table \"" + name +
                              "\". Name already exists.");
  }
  auto result = this->tables.emplace(name, std::move(table));
  table_cv.notify_all();
  return *(result.first->second);
}

auto Database::lookupTable(const std::string &name,
                           bool waitIfMissing) -> Table * {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto iterator = this->tables.find(name);
  if (iterator != tables.end()) {
    return iterator->second.get();
  }

  if (!waitIfMissing) {
    return nullptr;
  }

  lock.unlock();
  std::unique_lock<std::shared_mutex> unique_lock(mutex_);
  table_cv.wait(unique_lock, [&] { return tables.find(name) != tables.end(); });
  return tables[name].get();
}

auto Database::operator[](const std::string &tableName) -> Table & {
  const std::shared_lock<std::shared_mutex> lock(mutex_);
  auto iterator = this->tables.find(tableName);
  if (iterator == this->tables.end()) {
    throw TableNameNotFound("Error accesing table \"" + tableName +
                            "\". Table not found.");
  }
  return *(iterator->second);
}

auto Database::operator[](const std::string &tableName) const -> const Table & {
  const std::shared_lock<std::shared_mutex> lock(mutex_);
  auto iterator = this->tables.find(tableName);
  if (iterator == this->tables.end()) {
    throw TableNameNotFound("Error accesing table \"" + tableName +
                            "\". Table not found.");
  }
  return *(iterator->second);
}

void Database::dropTable(const std::string &tableName) {
  const std::unique_lock<std::shared_mutex> lock(mutex_);
  auto iterator = this->tables.find(tableName);
  if (iterator == this->tables.end()) {
    throw TableNameNotFound("Error when trying to drop table \"" + tableName +
                            "\". Table not found.");
  }
  this->tables.erase(iterator);
}

void Database::printAllTable() {
  const std::shared_lock<std::shared_mutex> lock(mutex_);
  const int width = 15;
  std::cout << "Database overview:" << '\n';
  std::cout << "=========================" << '\n';
  std::cout << std::setw(width) << "Table name";
  std::cout << std::setw(width) << "# of fields";
  std::cout << std::setw(width) << "# of entries" << '\n';
  for (const auto &table : this->tables) {
    std::cout << std::setw(width) << table.first;
    std::cout << std::setw(width) << (*table.second).field().size() + 1;
    std::cout << std::setw(width) << (*table.second).size() << '\n';
  }
  std::cout << "Total " << this->tables.size() << " tables." << '\n';
  std::cout << "=========================" << '\n';
}

auto Database::getInstance() -> Database & {
  static std::shared_mutex instanceMutex;
  const std::unique_lock<std::shared_mutex> lock(instanceMutex);
  if (Database::instance == nullptr) {
    instance = std::unique_ptr<Database>(new Database);
  }
  return *instance;
}

void Database::updateFileTableName(const std::string &fileName,
                                   const std::string &tableName) {
  const std::unique_lock<std::shared_mutex> lock(mutex_file);
  fileTableNameMap[fileName] = tableName;
}

auto Database::getFileTableName(const std::string &fileName) -> std::string {
  const std::unique_lock<std::shared_mutex> lock(mutex_file);
  auto iterator = fileTableNameMap.find(fileName);
  if (iterator == fileTableNameMap.end()) {
    std::ifstream infile(fileName);
    if (!infile.is_open()) {
      return "";
    }
    std::string tableNameFromFile;
    infile >> tableNameFromFile;
    infile.close();
    fileTableNameMap.emplace(fileName, tableNameFromFile);
    return tableNameFromFile;
  }
  return iterator->second;
}

auto Database::loadTableFromStream(std::istream &inputStream,
                                   const std::string &source) -> Table & {
  auto &database = Database::getInstance();
  const std::string errorString =
      !source.empty() ? R"(Invalid table (from "?") format: )"_f % source
                      : "Invalid table format: ";

  std::string tableName;
  Table::SizeType fieldCount = 0;
  std::deque<Table::KeyType> fields;

  std::string line;
  std::stringstream stringStream;
  if (!std::getline(inputStream, line)) {
    throw LoadFromStreamException(errorString +
                                  "Failed to read table metadata line.");
  }

  stringStream.str(line);
  stringStream >> tableName >> fieldCount;
  if (!stringStream) {
    throw LoadFromStreamException(errorString +
                                  "Failed to parse table metadata.");
  }

  database.testDuplicate(tableName);

  if (!(std::getline(inputStream, line))) {
    throw LoadFromStreamException(errorString + "Failed to load field names.");
  }

  stringStream.clear();
  stringStream.str(line);
  for (Table::SizeType index = 0; index < fieldCount; ++index) {
    std::string field;
    if (!(stringStream >> field)) {
      throw LoadFromStreamException(errorString +
                                    "Failed to load field names.");
    }
    fields.emplace_back(std::move(field));
  }

  if (fields.front() != "KEY") {
    throw LoadFromStreamException(errorString +
                                  "Missing or invalid KEY field.");
  }

  fields.erase(fields.begin());
  auto table = std::make_unique<Table>(tableName, fields);

  Table::SizeType lineCount = 2;
  while (std::getline(inputStream, line)) {
    if (line.empty()) {
      break;
    }
    lineCount++;
    stringStream.clear();
    stringStream.str(line);
    std::string key;
    if (!(stringStream >> key)) {
      throw LoadFromStreamException(errorString +
                                    "Missing or invalid KEY field.");
    }
    std::vector<Table::ValueType> tuple;
    tuple.reserve(fieldCount - 1);
    for (Table::SizeType index = 1; index < fieldCount; ++index) {
      Table::ValueType value = 0;
      if (!(stringStream >> value)) {
        throw LoadFromStreamException(errorString + "Invalid row on LINE " +
                                      std::to_string(lineCount));
      }
      tuple.emplace_back(value);
    }
    table->insertByIndex(key, std::move(tuple));
  }

  return database.registerTable(std::move(table));
}

void Database::exit() {}
