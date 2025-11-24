//
// Created by liu on 18-10-25.
//

#include "DumpTableQuery.h"

#include <exception>
#include <fstream>
#include <memory>
#include <string>

#include "../../utils/formatter.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto DumpTableQuery::execute() -> QueryResult::Ptr {
  const Database &database = Database::getInstance();
  try {
    std::ofstream outfile(this->fileName);
    if (!outfile.is_open()) {
      return std::make_unique<ErrorMsgResult>(qname, "Cannot open file '?'"_f %
                                                         this->fileName);
    }
    outfile << database[this->targetTable];
    outfile.close();
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

auto DumpTableQuery::toString() -> std::string {
  return "QUERY = Dump TABLE, FILE = \"" + this->fileName + "\"";
}
