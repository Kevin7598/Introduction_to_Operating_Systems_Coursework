//
// Created by XiMuzhe on 25-10-24.
//

#include "SubQuery.h"

#include <cstddef>
#include <exception>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../../db/Table.h"
#include "../QueryResult.h"

auto SubQuery::execute() -> QueryResult::Ptr {
  // SUB query requires at least 2 operands: fieldSrc and destField
  if (this->operands.size() < 2) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands). Must have at least 2."_f %
            operands.size());
  }

  Database &database = Database::getInstance();

  try {
    auto &table = database[this->targetTable];
    std::vector<Table::FieldIndex> fieldIds;

    // Extract field indices for all operands
    for (const auto &operand : this->operands) {
      auto fieldId = table.getFieldIndex(operand);
      fieldIds.push_back(fieldId);
    }

    // Initialize condition evaluation
    auto result = initCondition(table);
    if (result.second) {
      // First operand is fieldSrc, last operand is destField
      const Table::FieldIndex srcFieldId = fieldIds[0];
      const Table::FieldIndex destFieldId = fieldIds[fieldIds.size() - 1];

      // Middle operands are fields to subtract (can be zero or more)
      std::vector<Table::FieldIndex> subtractFieldIds;
      for (size_t i = 1; i < fieldIds.size() - 1; ++i) {
        subtractFieldIds.push_back(fieldIds[i]);
      }

      Table::SizeType counter = 0;
      // Iterate through all records in the table
      for (const auto &record : table) {
        // Check if record satisfies WHERE conditions
        if (this->evalCondition(record)) {
          // Calculate subtraction: fieldSrc - (field1 + field2 + ...)
          Table::ValueType resultValue = record[srcFieldId];

          // Subtract all specified field values using std::accumulate
          resultValue = std::accumulate(
              subtractFieldIds.begin(), subtractFieldIds.end(), resultValue,
              [&record](Table::ValueType acc, Table::FieldIndex fieldId) {
                return acc - record[fieldId];
              });

          // Store result in destination field
          record[destFieldId] = resultValue;
          counter++;
        }
      }
      return std::make_unique<RecordCountResult>(counter);
    }

    // Return count of affected rows
    return std::make_unique<RecordCountResult>(0);
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

auto SubQuery::toString() -> std::string {
  return "QUERY = SUB \"" + this->targetTable + "\"";
}
