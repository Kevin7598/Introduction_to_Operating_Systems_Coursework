//
// Created by pan on 2025/10/25.
//

#ifndef SRC_QUERY_DATA_TRUNCATEQUERY_H_
#define SRC_QUERY_DATA_TRUNCATEQUERY_H_

#include <string>

#include "../Query.h"

class TruncateQuery : public Query {
  static constexpr const char *qname = "TRUNCATE";

public:
  using Query::Query;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;
};

#endif  // SRC_QUERY_DATA_TRUNCATEQUERY_H_