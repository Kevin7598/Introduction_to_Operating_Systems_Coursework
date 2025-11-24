//
// Created by GuYuwei on 25-10-23.
//

#ifndef SRC_QUERY_DATA_SUMQUERY_H_
#define SRC_QUERY_DATA_SUMQUERY_H_

#include <string>

#include "../Query.h"

class SumQuery : public ComplexQuery {
  static constexpr const char *qname = "SUM";

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;

  [[nodiscard]] auto isReadOnly() const -> bool override { return true; }
};

#endif  // SRC_QUERY_DATA_SUMQUERY_H_
