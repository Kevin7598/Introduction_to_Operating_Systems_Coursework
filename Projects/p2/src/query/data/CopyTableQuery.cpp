//
// Created by pan on 2025/10/25.
//

#include "CopyTableQuery.h"

#include <exception>
#include <memory>
#include <string>
#include <utility>

#include "../../utils/uexception.h"
#include "../QueryResult.h"

#include "../../db/Database.h"
#include "../../db/Table.h"

auto CopyTableQuery::execute() -> QueryResult::Ptr {
  Database &database = Database::getInstance();
  try {
    // Ensure source table exists
    auto &sourceTable = database[this->targetTable];

    // Check if destination table already exists
    bool exists = true;
    try {
      (void)database[this->newTableName];
    } catch (const TableNameNotFound &) {
      exists = false;
    }
    if (exists) {
      return std::make_unique<ErrorMsgResult>(qname, newTableName,
                                              "Table already exists.");
    }

    // Create a new table by copying the source, with a new name
    auto newTable = std::make_unique<Table>(newTableName, sourceTable);
    database.registerTable(std::move(newTable));

    return std::make_unique<SuccessMsgResult>(
        targetTable, qname, "Copied to new table '" + newTableName + "'");
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "Source table not found.");
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable, e.what());
  }
}

auto CopyTableQuery::toString() -> std::string {
  return "QUERY = COPYTABLE, Source = \"" + targetTable + "\", New = \"" +
         newTableName + "\"";
}
