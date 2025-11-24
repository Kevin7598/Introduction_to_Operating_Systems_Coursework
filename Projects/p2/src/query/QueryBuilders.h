//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_QUERYBUILDERS_H_
#define SRC_QUERY_QUERYBUILDERS_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../utils/uexception.h"
#include "Query.h"  // NOLINT(misc-include-cleaner)
#include "QueryParser.h"

struct DebugTag {};
struct ManageTableTag {};
struct ComplexTag {};

template <typename Tag> struct QueryBuilderType;

class DebugQueryBuilder;
class ManageTableQueryBuilder;
class ComplexQueryBuilder;

template <> struct QueryBuilderType<DebugTag> {
  using type = DebugQueryBuilder;
};

template <> struct QueryBuilderType<ManageTableTag> {
  using type = ManageTableQueryBuilder;
};

template <> struct QueryBuilderType<ComplexTag> {
  using type = ComplexQueryBuilder;
};

template <typename Tag>
constexpr auto makeQueryBuilder() -> typename QueryBuilderType<Tag>::type {
  return typename QueryBuilderType<Tag>::type{};
}

class FailedQueryBuilder : public QueryBuilder {
public:
  static auto getDefault() { return std::make_unique<FailedQueryBuilder>(); }

  auto tryExtractQuery(const TokenizedQueryString &query)
      -> Query::Ptr final {  // const reference (cpplint runtime/references)
    throw QueryBuilderMatchFailed(query.rawQeuryString);
  }

  void setNext([[maybe_unused]] QueryBuilder::Ptr &&builder) final {}

  void clear() override {}
};

class BasicQueryBuilder : public QueryBuilder {
protected:
  QueryBuilder::Ptr
      nextBuilder  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
      {std::make_unique<FailedQueryBuilder>()};

public:
  BasicQueryBuilder() = default;

  void setNext(Ptr &&builder) override { nextBuilder = std::move(builder); }

  auto tryExtractQuery(const TokenizedQueryString &query)
      -> Query::Ptr override {  // const reference
    return nextBuilder->tryExtractQuery(query);
  }

  void clear() override { nextBuilder->clear(); }
};

class ComplexQueryBuilder : public BasicQueryBuilder {
protected:
  std::string
      targetTable;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
  std::vector<std::string>
      operandToken;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
  std::vector<QueryCondition>
      conditionToken;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,misc-non-private-member-variables-in-classes)
  virtual void parseToken(const TokenizedQueryString &query);

public:
  ComplexQueryBuilder() = default;
  void clear() override;

  // Used as a debugging function.
  // Prints the parsed information
  auto tryExtractQuery(const TokenizedQueryString &query)
      -> Query::Ptr override;  // const reference

private:
  auto parseOperand(std::vector<std::string>::const_iterator iterator,
                    const std::vector<std::string>::const_iterator &end)
      -> std::vector<std::string>::const_iterator;

  auto parseFromClause(std::vector<std::string>::const_iterator iterator,
                       const std::vector<std::string>::const_iterator &end)
      -> std::vector<std::string>::const_iterator;

  void parseWhereClause(std::vector<std::string>::const_iterator iterator,
                        const std::vector<std::string>::const_iterator &end);
};

class FakeQueryBuilder : public BasicQueryBuilder {
public:
  auto
  tryExtractQuery(const TokenizedQueryString &query) -> Query::Ptr override;
};

class DebugQueryBuilder : public BasicQueryBuilder {
public:
  auto
  tryExtractQuery(const TokenizedQueryString &query) -> Query::Ptr override;
};

class ManageTableQueryBuilder : public BasicQueryBuilder {
public:
  auto
  tryExtractQuery(const TokenizedQueryString &query) -> Query::Ptr override;
};

#endif  // SRC_QUERY_QUERYBUILDERS_H_
