//
// Created by pan on 2025/10/25.
//

#include "SwapQuery.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto SwapQuery::execute() -> QueryResult::Ptr {
  // SWAP query requires exactly 2 operands (fields to swap)
  if (this->operands.size() != 2) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable.c_str(),
        "Invalid number of operands (? operands). Must have exactly 2."_f %
            operands.size());
  }

  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    // Get field indices for the two operands
    auto field1Id = table.getFieldIndex(operands[0]);
    auto field2Id = table.getFieldIndex(operands[1]);

    // Initialize condition evaluation
    auto result = initCondition(table);
    if (result.second) {
      Table::SizeType counter = 0;
      // Iterate through all records
      for (const auto &record : table) {
        // Check if record satisfies WHERE conditions
        if (this->evalCondition(record)) {
          // Swap the two fields' values
          std::swap(record[field1Id], record[field2Id]);
          counter++;
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
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'."_f % e.what());
  }
}

auto SwapQuery::toString() -> std::string {
  return "QUERY = SWAP (" + operands[0] + " " + operands[1] + ") FROM \"" +
         this->targetTable + "\"";
}
