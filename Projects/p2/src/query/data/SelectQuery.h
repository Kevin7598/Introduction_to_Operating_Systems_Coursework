//
// Created by kaiqizhu on 25/10/23.
//

#ifndef SRC_QUERY_DATA_SELECTQUERY_H_
#define SRC_QUERY_DATA_SELECTQUERY_H_

#include <string>
#include <utility>
#include <vector>

#include "../Query.h"

class SelectQuery : public ComplexQuery {
  static constexpr const char *qname = "SELECT";

public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  SelectQuery(std::string table, std::vector<std::string> operandtoken,
              std::vector<QueryCondition> conds)
      : ComplexQuery(std::move(table), std::move(operandtoken),
                     std::move(conds)) {}

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override {
    return "QUERY = SELECT FROM " + targetTable;
  }

  [[nodiscard]] auto isReadOnly() const -> bool override { return true; }
};

#endif  // SRC_QUERY_DATA_SELECTQUERY_H_
