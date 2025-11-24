//
// Created by liu on 18-10-25.
//

#include "PrintTableQuery.h"

#include <iostream>
#include <memory>
#include <string>

#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto PrintTableQuery::execute() -> QueryResult::Ptr {
  Database &database = Database::getInstance();
  try {
    auto &table = database[this->targetTable];
    std::cout << "================\n";
    std::cout << "TABLE = ";
    std::cout << table;
    std::cout << "================\n";
    return std::make_unique<SuccessMsgResult>(qname, this->targetTable);
  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "No such table.");
  }
}

auto PrintTableQuery::toString() -> std::string {
  return "QUERY = SHOWTABLE, Table = \"" + this->targetTable + "\"";
}
