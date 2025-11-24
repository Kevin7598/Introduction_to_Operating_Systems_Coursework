//
// Created by XiMuZhe on 25-10-24.
//

#include "MaxQuery.h"

#include <cstddef>
#include <exception>
#include <limits>
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

auto MaxQuery::execute() -> QueryResult::Ptr {
  if (this->operands.empty()) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  }

  Database &database = Database::getInstance();

  try {
    auto &table = database[this->targetTable];
    std::vector<Table::FieldIndex> fieldIds;

    // Extract field indices for operands
    for (const auto &operand : this->operands) {
      auto fieldId = table.getFieldIndex(operand);
      fieldIds.push_back(fieldId);
    }

    // Initialize condition and Max values
    auto result = initCondition(table);
    std::vector<Table::ValueType> maxValues(
        fieldIds.size(), std::numeric_limits<Table::ValueType>::min());
    bool hasRecords = false;

    if (result.second) {
      for (const auto &record : table) {
        if (this->evalCondition(record)) {
          hasRecords = true;
          for (size_t i = 0; i < fieldIds.size(); ++i) {
            const Table::ValueType currentValue = record[fieldIds[i]];
            if (currentValue > maxValues[i]) {
              maxValues[i] = currentValue;
            }
          }
        }
      }
    }

    // If no records match the condition, return empty result
    if (!hasRecords) {
      return std::make_unique<NullQueryResult>();
    }

    return std::make_unique<SuccessMsgResult>(maxValues);
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

auto MaxQuery::toString() -> std::string {
  return "QUERY = MAX \"" + this->targetTable + "\"";
}
