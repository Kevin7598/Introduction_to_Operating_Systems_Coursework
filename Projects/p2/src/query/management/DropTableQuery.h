//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_MANAGEMENT_DROPTABLEQUERY_H_
#define SRC_QUERY_MANAGEMENT_DROPTABLEQUERY_H_

#include <string>

#include "../Query.h"

class DropTableQuery : public Query {
  static constexpr const char *qname = "DROP";

public:
  using Query::Query;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;

  // For scheduler: dropping a table is a DB-level operation
  [[nodiscard]] auto isDbLevel() const -> bool override { return true; }
};

#endif  // SRC_QUERY_MANAGEMENT_DROPTABLEQUERY_H_
