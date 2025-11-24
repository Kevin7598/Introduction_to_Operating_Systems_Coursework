#ifndef SRC_QUERY_QUERYPARSER_H_
#define SRC_QUERY_QUERYPARSER_H_

#include <memory>
#include <string>
#include <vector>

#include "Query.h"  // NOLINT(misc-include-cleaner)

struct TokenizedQueryString {
  std::vector<std::string> token;
  std::string rawQeuryString;
};

class QueryBuilder {
public:
  using Ptr = std::unique_ptr<QueryBuilder>;

  QueryBuilder() = default;
  virtual ~QueryBuilder() = default;

  virtual auto tryExtractQuery(const TokenizedQueryString &queryString)
      -> Query::Ptr = 0;  // Converted to const reference (cpplint
                          // runtime/references)
  virtual void setNext(Ptr &&builder) = 0;
  virtual void clear() = 0;

  QueryBuilder(const QueryBuilder &) = delete;
  auto operator=(const QueryBuilder &) = delete;

  QueryBuilder(QueryBuilder &&) = delete;
  auto operator=(QueryBuilder &&) = delete;
};

class QueryParser {
  QueryBuilder::Ptr first;
  QueryBuilder *last = nullptr;

  static auto
  tokenizeQueryString(const std::string &queryString) -> TokenizedQueryString;

public:
  auto parseQuery(const std::string &queryString) -> Query::Ptr;
  void registerQueryBuilder(QueryBuilder::Ptr &&qBuilder);

  QueryParser();
};

#endif  // SRC_QUERY_QUERYPARSER_H_
