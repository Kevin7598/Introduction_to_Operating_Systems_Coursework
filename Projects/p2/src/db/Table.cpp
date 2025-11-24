//
// Created by liu on 18-10-23.
//

#include "Table.h"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../utils/formatter.h"
#include "../utils/uexception.h"

auto Table::getFieldIndex(const Table::FieldNameType &field) const
    -> Table::FieldIndex {
  const std::shared_lock<std::shared_mutex> lock(mutex_);
  try {
    return this->fieldMap.at(field);
  } catch (const std::out_of_range &exception) {
    throw TableFieldNotFound(R"(Field name "?" doesn't exists.)"_f % (field));
  }
}

void Table::insertByIndex(const KeyType &key, std::vector<ValueType> &&values) {
  const std::unique_lock<std::shared_mutex> lock(mutex_);
  if (this->keyMap.find(key) != this->keyMap.end()) {
    const std::string errorMessage = "In Table \"" + this->tableName +
                                     "\" : Key \"" + key + "\" already exists!";
    throw ConflictingKey(errorMessage);
  }
  this->keyMap.emplace(key, this->data.size());
  this->data.emplace_back(key, std::move(values));
}

auto Table::erase(const KeyType &key) -> bool {
  const std::unique_lock<std::shared_mutex> lock(mutex_);
  auto iterator = keyMap.find(key);
  if (iterator == keyMap.end()) {
    return false;
  }

  const auto index = static_cast<int64_t>(iterator->second);

  data.erase(data.begin() + index);

  keyMap.erase(iterator);

  for (auto &pair : keyMap) {
    if (pair.second > static_cast<std::size_t>(index)) {
      pair.second--;
    }
  }

  return true;
}

auto Table::eraseMany(const std::vector<KeyType> &keys) -> SizeType {
  std::unordered_set<KeyType> to_delete(keys.begin(), keys.end());
  const std::unique_lock<std::shared_mutex> lock(mutex_);

  SizeType removed = 0;
  std::vector<Datum> newdata;
  newdata.reserve(data.size());
  std::unordered_map<KeyType, SizeType> newKeyMap;

  for (const auto &del : data) {
    if (to_delete.find(del.key()) == to_delete.end()) {
      newKeyMap.emplace(del.key(), newdata.size());
      newdata.push_back(del);
    } else {
      ++removed;
    }
  }

  data.swap(newdata);
  keyMap.swap(newKeyMap);

  return removed;
}

auto Table::operator[](const Table::KeyType &key) -> Table::Object::Ptr {
  const std::shared_lock<std::shared_mutex> lock(mutex_);
  auto iterator = keyMap.find(key);
  if (iterator == keyMap.end()) {
    return nullptr;
  }
  return createProxy(
      data.begin() + static_cast<std::vector<Table::Datum>::difference_type>(
                         iterator->second),
      this);
}

auto operator<<(std::ostream &outputStream,
                const Table &table) -> std::ostream & {
  const std::shared_lock<std::shared_mutex> lock(table.mutex_);
  const int width = 10;
  std::stringstream buffer;
  buffer << table.tableName << "\t" << (table.fields.size() + 1) << "\n";
  buffer << std::setw(width) << "KEY";
  for (const auto &field : table.fields) {
    buffer << std::setw(width) << field;
  }
  buffer << "\n";
  auto numFields = table.fields.size();
  for (const auto &datum : table.data) {
    buffer << std::setw(width) << datum.key();
    for (decltype(numFields) index = 0; index < numFields; ++index) {
      buffer << std::setw(width) << datum.datum()[index];
    }
    buffer << "\n";
  }
  return outputStream << buffer.str();
}
