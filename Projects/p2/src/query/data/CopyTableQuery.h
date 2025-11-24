//
// Created by pan on 2025/10/25.
//

//
// Created by [Your Name] on [Date].
//

#ifndef SRC_QUERY_DATA_COPYTABLEQUERY_H_
#define SRC_QUERY_DATA_COPYTABLEQUERY_H_

#include <string>
#include <utility>

#include "../Query.h"

class CopyTableQuery : public Query {
  static constexpr const char *qname = "COPYTABLE";
  std::string newTableName;

public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  CopyTableQuery(std::string sourceTable, std::string newTable)
      : Query(std::move(sourceTable)), newTableName(std::move(newTable)) {}

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;

  // For scheduler: copying creates a new table, DB-level operation
  [[nodiscard]] auto isDbLevel() const -> bool override { return true; }
};

#endif  // SRC_QUERY_DATA_COPYTABLEQUERY_H_