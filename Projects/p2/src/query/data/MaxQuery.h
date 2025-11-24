//
// Created by XiMuZhe on 25-10-24.
//

#ifndef SRC_QUERY_DATA_MAXQUERY_H_
#define SRC_QUERY_DATA_MAXQUERY_H_

#include <string>

#include "../Query.h"

class MaxQuery : public ComplexQuery {
  static constexpr const char *qname = "MAX";

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;

  [[nodiscard]] auto isReadOnly() const -> bool override { return true; }
};

#endif  // SRC_QUERY_DATA_MAXQUERY_H_