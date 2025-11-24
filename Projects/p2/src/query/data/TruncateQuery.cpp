//
// Created by pan on 2025/10/25.
//

#include "TruncateQuery.h"

#include <exception>
#include <memory>
#include <string>

#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto TruncateQuery::execute() -> QueryResult::Ptr {
  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    table.clear();
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, targetTable,
                                            "No such table.");
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

auto TruncateQuery::toString() -> std::string {
  return "QUERY = TRUNCATE, Table = \"" + targetTable + "\"";
}
