//
// Created by GuYuwei on 25-10-23.
//

#include "SumQuery.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../../db/Table.h"
#include "../QueryResult.h"

auto SumQuery::execute() -> QueryResult::Ptr {
  if (this->operands.empty()) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands)."_f % operands.size());
  }
  Database &database = Database::getInstance();
  std::vector<Table::ValueType> sums(this->operands.size(), 0);
  try {
    auto &table = database[this->targetTable];
    std::vector<Table::FieldIndex> fieldIds;

    // Extract field indices for operands
    for (const auto &operand : this->operands) {
      auto fieldId = table.getFieldIndex(operand);
      fieldIds.push_back(fieldId);
    }

    // Initialize condition and Sum values
    auto result = initCondition(table);
    if (result.second) {
      for (const auto &record : table) {
        if (this->evalCondition(record)) {
          for (size_t i = 0; i < fieldIds.size(); ++i) {
            sums[i] += record[fieldIds[i]];
          }
        }
      }
    }

    return std::make_unique<SuccessMsgResult>(sums);
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

auto SumQuery::toString() -> std::string {
  return "QUERY = SUM \"" + this->targetTable + "\"";
}
