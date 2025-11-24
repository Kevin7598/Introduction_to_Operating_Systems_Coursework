//
// Created by liu on 18-10-25.
//

#include "InsertQuery.h"

#include <cstdlib>
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

auto InsertQuery::execute() -> QueryResult::Ptr {
  if (this->operands.empty()) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable.c_str(),
                                            "No operand (? operands)."_f %
                                                operands.size());
  }
  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    auto &key = this->operands.front();
    std::vector<Table::ValueType> data;
    data.reserve(this->operands.size() - 1);
    for (auto it = ++this->operands.begin(); it != this->operands.end(); ++it) {
      constexpr int decimalBase = 10;
      data.emplace_back(std::strtol(it->c_str(), nullptr, decimalBase));
    }
    table.insertByIndex(key, std::move(data));
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "No such table.");
  } catch (const IllFormedQueryCondition &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const std::invalid_argument &e) {
    // Cannot convert operand to string
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unkonwn error '?'."_f % e.what());
  }
}

auto InsertQuery::toString() -> std::string {
  return "QUERY = INSERT \"" + this->targetTable + "\"";
}
