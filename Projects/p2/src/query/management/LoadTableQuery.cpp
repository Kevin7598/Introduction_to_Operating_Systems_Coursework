//
// Created by liu on 18-10-25.
//

#include "LoadTableQuery.h"

#include <exception>
#include <fstream>
#include <memory>
#include <string>

#include "../../utils/formatter.h"

#include "../../db/Database.h"
#include "../QueryResult.h"

auto LoadTableQuery::execute() -> QueryResult::Ptr {
  try {
    std::ifstream infile(this->fileName);
    if (!infile.is_open()) {
      return std::make_unique<ErrorMsgResult>(qname, "Cannot open file '?'"_f %
                                                         this->fileName);
    }
    Database::loadTableFromStream(infile, this->fileName);
    infile.close();
    return std::make_unique<SuccessMsgResult>(qname, targetTable);
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(qname, e.what());
  }
}

auto LoadTableQuery::toString() -> std::string {
  return "QUERY = Load TABLE, FILE = \"" + this->fileName + "\"";
}
