//
// Created by XiMuzhe on 25-10-24.
//

#ifndef SRC_QUERY_DATA_SUBQUERY_H_
#define SRC_QUERY_DATA_SUBQUERY_H_

#include <string>

#include "../Query.h"

class SubQuery : public ComplexQuery {
  static constexpr const char *qname = "SUB";

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;
};

#endif  // SRC_QUERY_DATA_SUBQUERY_H_
