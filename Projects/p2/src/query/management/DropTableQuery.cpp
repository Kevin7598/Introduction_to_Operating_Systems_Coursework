//
// Created by liu on 18-10-25.
//

#include "DropTableQuery.h"

#include <exception>
#include <memory>
#include <string>

#include "../../utils/uexception.h"  // TableNameNotFound

#include "../../db/Database.h"
#include "../QueryResult.h"

auto DropTableQuery::execute() -> QueryResult::Ptr {
  Database &database = Database::getInstance();
  try {
    database.dropTable(this->targetTable);
    return std::make_unique<SuccessMsgResult>(qname);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "No such table.");
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

auto DropTableQuery::toString() -> std::string {
  return "QUERY = DROP, Table = \"" + targetTable + "\"";
}
