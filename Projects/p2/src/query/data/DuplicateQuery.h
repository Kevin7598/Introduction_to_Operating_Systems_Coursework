//
// Created by GuYuwei on 25-10-23.
//

#ifndef SRC_QUERY_DATA_DUPLICATEQUERY_H_
#define SRC_QUERY_DATA_DUPLICATEQUERY_H_

#include <string>
#include <utility>  // for std::pair
#include <vector>   // for std::vector

#include "../Query.h"

class DuplicateQuery : public ComplexQuery {
  static constexpr const char *qname = "DUPLICATE";

private:
  // Collect rows (values + original keys) that should be duplicated.
  // Skips originals that already have any copy variant.
  auto collectDataToCopy(Table &table)  // NOLINT(runtime/references)
      -> std::pair<std::vector<std::vector<Table::ValueType>>,
                   std::vector<Table::KeyType>>;

  // Generate a unique key by appending "_copy" repeatedly until unused.
  static auto
  generateUniqueKey(Table &table,             // NOLINT(runtime/references)
                    const std::string &base)  // NOLINT(runtime/references)
      -> std::string;

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;
};

#endif  // SRC_QUERY_DATA_DUPLICATEQUERY_H_
