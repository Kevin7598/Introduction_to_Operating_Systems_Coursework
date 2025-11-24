//
// Created by liu on 18-10-23.
//

#ifndef SRC_DB_DATABASE_H_
#define SRC_DB_DATABASE_H_

#include <condition_variable>
#include <istream>
#include <memory>
#include <shared_mutex>
#include <string>
#include <unordered_map>

#include "Table.h"

class Database {
private:
  static std::unique_ptr<Database> instance;

  mutable std::shared_mutex mutex_;
  mutable std::shared_mutex mutex_file;
  std::condition_variable_any table_cv;
  std::unordered_map<std::string, Table::Ptr> tables;
  std::unordered_map<std::string, std::string> fileTableNameMap;

  Database() = default;

public:
  void testDuplicate(const std::string &tableName);

  auto registerTable(Table::Ptr &&table) -> Table &;

  void dropTable(const std::string &tableName);

  auto lookupTable(const std::string &name,
                   bool waitIfMissing = false) -> Table *;

  void printAllTable();

  auto operator[](const std::string &tableName) -> Table &;

  auto operator[](const std::string &tableName) const -> const Table &;

  auto operator=(const Database &) -> Database & = delete;

  auto operator=(Database &&) -> Database & = delete;

  Database(const Database &) = delete;
  Database(Database &&) = delete;
  ~Database() = default;

  static auto getInstance() -> Database &;

  void updateFileTableName(const std::string &fileName,
                           const std::string &tableName);

  auto getFileTableName(const std::string &fileName) -> std::string;

  static auto loadTableFromStream(std::istream &inputStream,
                                  const std::string &source = "") -> Table &;

  static void exit();
};

#endif  // SRC_DB_DATABASE_H_
