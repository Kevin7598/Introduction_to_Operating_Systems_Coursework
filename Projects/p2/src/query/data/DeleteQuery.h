//
// Created by kaiqizhu on 25/10/23.
//

#ifndef SRC_QUERY_DATA_DELETEQUERY_H_
#define SRC_QUERY_DATA_DELETEQUERY_H_

#include <string>
#include <utility>
#include <vector>

#include "../Query.h"

class DeleteQuery : public ComplexQuery {
  static constexpr const char *qname = "DELETE";

public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  DeleteQuery(std::string table, std::vector<std::string> operandtoken,
              std::vector<QueryCondition> conds)
      : ComplexQuery(std::move(table), std::move(operandtoken),
                     std::move(conds)) {}

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override {
    return "QUERY = DELETE FROM " + targetTable;
  }
};

#endif  // SRC_QUERY_DATA_DELETEQUERY_H_
