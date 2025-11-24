//
// Created by liu on 18-10-25.
//

#include "UpdateQuery.h"

#include <cstdlib>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto UpdateQuery::execute() -> QueryResult::Ptr {
  if (this->operands.size() != 2) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  }
  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    if (this->operands[0] == "KEY") {
      this->keyValue = this->operands[1];
    } else {
      this->fieldId = table.getFieldIndex(this->operands[0]);
      constexpr int decimalBase = 10;
      this->fieldValue = (Table::ValueType)std::strtol(
          this->operands[1].c_str(), nullptr, decimalBase);
    }
    auto result = initCondition(table);
    if (result.second) {
      Table::SizeType counter = 0;
      for (auto it = table.begin(); it != table.end(); ++it) {
        if (this->evalCondition(*it)) {
          if (this->keyValue.empty()) {
            (*it)[this->fieldId] = this->fieldValue;
          } else {
            it->setKey(this->keyValue);
          }
          ++counter;
        }
      }
      return std::make_unique<RecordCountResult>(counter);
    }
    return std::make_unique<RecordCountResult>(0);
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

auto UpdateQuery::toString() -> std::string {
  return "QUERY = UPDATE \"" + this->targetTable + "\"";
}
