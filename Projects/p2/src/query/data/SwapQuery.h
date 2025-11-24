//
// Created by 23723 on 2025/10/25.
//

#ifndef SRC_QUERY_DATA_SWAPQUERY_H_
#define SRC_QUERY_DATA_SWAPQUERY_H_

#include <string>

#include "../Query.h"

class SwapQuery : public ComplexQuery {
  static constexpr const char *qname = "SWAP";

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;
};

#endif  // SRC_QUERY_DATA_SWAPQUERY_H_
