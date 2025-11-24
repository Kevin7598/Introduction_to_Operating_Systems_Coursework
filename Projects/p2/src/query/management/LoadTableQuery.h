//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_MANAGEMENT_LOADTABLEQUERY_H_
#define SRC_QUERY_MANAGEMENT_LOADTABLEQUERY_H_

#include <string>
#include <utility>

#include "../Query.h"

class LoadTableQuery : public Query {
  static constexpr const char *qname = "LOAD";
  std::string fileName;

public:
  // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
  explicit LoadTableQuery(std::string table, std::string fileName)
      : Query(std::move(table)), fileName(std::move(fileName)) {}

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;

  // For scheduler
  [[nodiscard]] auto isLoad() const -> bool override { return true; }
  [[nodiscard]] auto isDbLevel() const -> bool override { return true; }
};

#endif  // SRC_QUERY_MANAGEMENT_LOADTABLEQUERY_H_
