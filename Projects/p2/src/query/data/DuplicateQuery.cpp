//
// Created by GuYuwei on 25-10-23.
//

#include "DuplicateQuery.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../../db/Table.h"
#include "../QueryResult.h"

auto DuplicateQuery::collectDataToCopy(Table &table)
    -> std::pair<std::vector<std::vector<Table::ValueType>>,
                 std::vector<Table::KeyType>> {
  std::vector<std::vector<Table::ValueType>> dataToCopy;
  std::vector<Table::KeyType> originalKeys;
  const std::string copySuffix = "_copy";

  for (auto it = table.begin(); it != table.end(); ++it) {
    if (!this->evalCondition(*it)) {
      continue;
    }
    const auto &key = (*it).key();
    bool isOriginalWithCopy = false;
    std::string test_key = key + copySuffix;
    while (table[test_key] != nullptr) {
      isOriginalWithCopy = true;
      test_key += copySuffix;
    }
    if (isOriginalWithCopy) {
      continue;
    }
    std::vector<Table::ValueType> new_values;
    new_values.reserve(table.field().size());
    for (const auto &field_name : table.field()) {
      new_values.push_back((*it)[field_name]);
    }
    dataToCopy.push_back(std::move(new_values));
    originalKeys.push_back(key);
  }
  return {std::move(dataToCopy), std::move(originalKeys)};
}

auto DuplicateQuery::generateUniqueKey(Table &table,
                                       const std::string &base) -> std::string {
  const std::string copySuffix = "_copy";
  const std::string new_key_base = base + copySuffix;
  std::string new_key = new_key_base;
  int suffix_count = 1;
  while (table[new_key] != nullptr) {
    new_key = new_key_base;
    for (int j = 0; j < suffix_count; ++j) {
      new_key += copySuffix;
    }
    ++suffix_count;
  }
  return new_key;
}

auto DuplicateQuery::execute() -> QueryResult::Ptr {
  if (!this->operands.empty()) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  }

  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    auto result = initCondition(table);
    if (!result.second) {
      return std::make_unique<RecordCountResult>(0);
    }

    auto [dataToCopy, originalKeys] = collectDataToCopy(table);
    Table::SizeType counter = 0;
    for (size_t i = 0; i < dataToCopy.size(); ++i) {
      auto uniqueKey = generateUniqueKey(table, originalKeys[i]);
      table.insertByIndex(uniqueKey, std::move(dataToCopy[i]));
      ++counter;
    }
    return std::make_unique<RecordCountResult>(counter);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "No such table.");
  } catch (const IllFormedQueryCondition &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const std::invalid_argument &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'."_f % e.what());
  }
}

auto DuplicateQuery::toString() -> std::string {
  return "QUERY = DUPLICATE, TABLE = \"" + this->targetTable + "\"";
}
