//
// Created by liu on 18-10-25.
//

#include "QueryBuilders.h"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../db/Database.h"
#include "../utils/formatter.h"
#include "../utils/uexception.h"
#include "Query.h"
#include "QueryParser.h"
#include "data/CopyTableQuery.h"
#include "data/DeleteQuery.h"
#include "data/DuplicateQuery.h"
#include "data/InsertQuery.h"
#include "data/MaxQuery.h"
#include "data/SelectQuery.h"
#include "data/SubQuery.h"
#include "data/SumQuery.h"
#include "data/SwapQuery.h"
#include "data/TruncateQuery.h"
#include "data/UpdateQuery.h"
#include "management/DropTableQuery.h"
#include "management/DumpTableQuery.h"
#include "management/ListTableQuery.h"
#include "management/LoadTableQuery.h"
#include "management/PrintTableQuery.h"
#include "management/QuitQuery.h"

enum class Constants : std::uint8_t { FIELD_WIDTH = 10, TOKENS_PER_LINE = 5 };
// Prints out debugging information.
// Does no real work
auto FakeQueryBuilder::tryExtractQuery(const TokenizedQueryString &query)
    -> std::unique_ptr<Query> {
  std::cerr << "Query string: \n" << query.rawQeuryString << "\n";
  std::cerr << "Tokens:\n";
  int count = 0;
  for (const auto &tok : query.token) {
    std::cerr << std::setw(static_cast<int>(Constants::FIELD_WIDTH)) << "\""
              << tok << "\"";
    count = (count + 1) % static_cast<int>(Constants::TOKENS_PER_LINE);
    if (count == static_cast<int>(Constants::TOKENS_PER_LINE) - 1) {
      std::cerr << "\n";
    }
  }
  if (count != static_cast<int>(Constants::TOKENS_PER_LINE) - 1) {
    std::cerr << "\n";
  }
  return this->nextBuilder->tryExtractQuery(query);
}

auto ManageTableQueryBuilder::tryExtractQuery(const TokenizedQueryString &query)
    -> std::unique_ptr<Query> {
  if (query.token.size() == 2) {
    if (query.token.front() == "LOAD") {
      auto &database = Database::getInstance();
      auto tableName = database.getFileTableName(query.token[1]);
      return std::make_unique<LoadTableQuery>(tableName, query.token[1]);
    }
    if (query.token.front() == "DROP") {
      return std::make_unique<DropTableQuery>(query.token[1]);
    }
    if (query.token.front() == "TRUNCATE") {
      return std::make_unique<TruncateQuery>(query.token[1]);
    }
  }
  if (query.token.size() == 3) {
    if (query.token.front() == "DUMP") {
      auto &dataBase = Database::getInstance();
      dataBase.updateFileTableName(query.token[2], query.token[1]);
      return std::make_unique<DumpTableQuery>(query.token[1], query.token[2]);
    }
    if (query.token.front() == "COPYTABLE") {
      return std::make_unique<CopyTableQuery>(query.token[1], query.token[2]);
    }
  }
  return this->nextBuilder->tryExtractQuery(query);
}

auto DebugQueryBuilder::tryExtractQuery(const TokenizedQueryString &query)
    -> std::unique_ptr<Query> {
  if (query.token.size() == 1) {
    if (query.token.front() == "LIST") {
      return std::make_unique<ListTableQuery>();
    }
    if (query.token.front() == "QUIT") {
      return std::make_unique<QuitQuery>();
    }
  }
  if (query.token.size() == 2) {
    if (query.token.front() == "SHOWTABLE") {
      return std::make_unique<PrintTableQuery>(query.token[1]);
    }
  }
  return BasicQueryBuilder::tryExtractQuery(query);
}

void ComplexQueryBuilder::parseToken(const TokenizedQueryString &query) {
  auto iterator = query.token.cbegin();
  const auto end = query.token.cend();

  ++iterator;
  if (iterator == end) {
    throw IllFormedQuery("Missing FROM clause");
  }

  iterator = ComplexQueryBuilder::parseOperand(iterator, end);

  iterator = ComplexQueryBuilder::parseFromClause(iterator, end);

  if (iterator != end) {
    ComplexQueryBuilder::parseWhereClause(iterator, end);
  }
}

auto ComplexQueryBuilder::parseOperand(
    std::vector<std::string>::const_iterator iterator,
    const std::vector<std::string>::const_iterator &end)
    -> std::vector<std::string>::const_iterator {
  if (*iterator != "FROM") {
    if (*iterator != "(") {
      throw IllFormedQuery("Ill-formed operand.");
    }
    ++iterator;

    while (iterator != end && *iterator != ")") {
      this->operandToken.push_back(*iterator);
      ++iterator;
    }

    if (iterator == end) {
      throw IllFormedQuery("Ill-formed operand");
    }

    ++iterator;
    if (iterator == end || *iterator != "FROM") {
      throw IllFormedQuery("Missing FROM clause");
    }
  }
  return iterator;
}

auto ComplexQueryBuilder::parseFromClause(
    std::vector<std::string>::const_iterator iterator,
    const std::vector<std::string>::const_iterator &end)
    -> std::vector<std::string>::const_iterator {
  ++iterator;
  if (iterator == end) {
    throw IllFormedQuery("Missing target table");
  }

  this->targetTable = *iterator;
  ++iterator;
  return iterator;
}

void ComplexQueryBuilder::parseWhereClause(
    std::vector<std::string>::const_iterator iterator,
    const std::vector<std::string>::const_iterator &end) {
  if (*iterator != "WHERE") {
    throw IllFormedQuery(R"(Expecting "WHERE", found "?".)"_f % *iterator);
  }

  ++iterator;
  while (iterator != end) {
    if (*iterator != "(") {
      throw IllFormedQuery("Ill-formed query condition");
    }

    QueryCondition cond;
    cond.fieldId = 0;
    cond.valueParsed = 0;

    if (++iterator == end) {
      throw IllFormedQuery("Missing field in condition");
    }
    cond.field = *iterator;

    if (++iterator == end) {
      throw IllFormedQuery("Missing operator in condition");
    }
    cond.op = *iterator;

    if (++iterator == end) {
      throw IllFormedQuery("Missing value in condition");
    }
    cond.value = *iterator;

    ++iterator;
    if (iterator == end || *iterator != ")") {
      throw IllFormedQuery("Ill-formed query condition");
    }

    this->conditionToken.push_back(cond);
    ++iterator;
  }
}

auto ComplexQueryBuilder::tryExtractQuery(const TokenizedQueryString &query)
    -> std::unique_ptr<Query> {
  try {
    this->parseToken(query);
  } catch (const IllFormedQuery &e) {
    std::cerr << e.what() << "\n";
    return this->nextBuilder->tryExtractQuery(query);
  }
  const std::string operation = query.token.front();
  if (operation == "INSERT") {
    return std::make_unique<InsertQuery>(this->targetTable, this->operandToken,
                                         this->conditionToken);
  }
  if (operation == "UPDATE") {
    return std::make_unique<UpdateQuery>(this->targetTable, this->operandToken,
                                         this->conditionToken);
  }
  if (operation == "SELECT") {
    return std::make_unique<SelectQuery>(this->targetTable, this->operandToken,
                                         this->conditionToken);
  }
  if (operation == "DELETE") {
    return std::make_unique<DeleteQuery>(this->targetTable, this->operandToken,
                                         this->conditionToken);
  }
  if (operation == "DUPLICATE") {
    return std::make_unique<DuplicateQuery>(
        this->targetTable, this->operandToken, this->conditionToken);
  }
  if (operation == "COUNT") {
    return std::make_unique<NopQuery>();
  }
  if (operation == "SUM") {
    return std::make_unique<SumQuery>(this->targetTable, this->operandToken,
                                      this->conditionToken);
  }
  if (operation == "MIN") {
    return std::make_unique<NopQuery>();
  }
  if (operation == "MAX") {
    return std::make_unique<MaxQuery>(this->targetTable, this->operandToken,
                                      this->conditionToken);
  }
  if (operation == "ADD") {
    return std::make_unique<NopQuery>();
  }
  if (operation == "SUB") {
    return std::make_unique<SubQuery>(this->targetTable, this->operandToken,
                                      this->conditionToken);
  }
  if (operation == "SWAP") {
    return std::make_unique<SwapQuery>(this->targetTable, this->operandToken,
                                       this->conditionToken);
  }
  std::cerr << "Complicated query found!" << "\n";
  std::cerr << "Operation = " << query.token.front() << "\n";
  std::cerr << "    Operands : ";
  for (const auto &oprand : this->operandToken) {
    std::cerr << oprand << " ";
  }
  std::cerr << "\n";
  std::cerr << "Target Table = " << this->targetTable << "\n";
  if (this->conditionToken.empty()) {
    std::cerr << "No WHERE clause specified." << "\n";
  } else {
    std::cerr << "Conditions = ";
  }
  for (const auto &cond : this->conditionToken) {
    std::cerr << cond.field << cond.op << cond.value << " ";
  }
  std::cerr << "\n";
  return this->nextBuilder->tryExtractQuery(query);
}

void ComplexQueryBuilder::clear() {
  this->conditionToken.clear();
  this->targetTable = "";
  this->operandToken.clear();
  this->nextBuilder->clear();
}
