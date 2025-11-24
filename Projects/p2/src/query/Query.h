//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_QUERY_H_
#define SRC_QUERY_QUERY_H_

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../db/Table.h"
// Needed for NullQueryResult and QueryResult::Ptr; keep despite include-cleaner
// warning.
#include "QueryResult.h"  // NOLINT(misc-include-cleaner)

struct QueryCondition {
  std::string field;
  size_t fieldId = 0;
  std::string op;
  std::function<bool(const Table::ValueType &, const Table::ValueType &)> comp;
  std::string value;
  Table::ValueType valueParsed{};
};

class Query {
protected:
  std::string
      targetTable;  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,
                    // misc-non-private-member-variables-in-classes)

public:
  Query() = default;

  explicit Query(std::string targetTable)
      : targetTable(std::move(targetTable)) {}

  using Ptr = std::unique_ptr<Query>;
  Query(const Query &) = default;
  Query(Query &&) noexcept = default;

  auto operator=(const Query &) -> Query & = default;
  auto operator=(Query &&) noexcept -> Query & = default;

  virtual auto execute() -> QueryResult::Ptr = 0;
  virtual auto toString() -> std::string = 0;

  /**
   * Indicate whether the query is read-only (does not modify schema or data).
   * Default: false. Read-only queries should override this to return true so
   * the caller can allow concurrent execution.
   */
  [[nodiscard]] virtual auto isReadOnly() const -> bool { return false; }

  // Scheduler helpers (default behaviors)
  // Expose target table for scheduling decisions
  [[nodiscard]] auto getTargetTable() -> std::string & { return targetTable; }

  // Identify LOAD queries
  [[nodiscard]] virtual auto isLoad() const -> bool { return false; }

  // Identify QUIT queries
  [[nodiscard]] virtual auto isQuit() const -> bool { return false; }

  // Identify database-level operations (affect DB schema/table set), e.g.,
  // LOAD/COPYTABLE/DROP. Default: false.
  [[nodiscard]] virtual auto isDbLevel() const -> bool { return false; }

  virtual ~Query() = default;
};

class NopQuery : public Query {
public:
  auto execute() -> QueryResult::Ptr override {
    return std::make_unique<NullQueryResult>();
  }

  auto toString() -> std::string
      override {  // NOLINT(readability-convert-member-functions-to-static)
    return "QUERY = NOOP";
  }
};

class ComplexQuery : public Query {
protected:
  std::vector<std::string>
      operands{};  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,
                   // misc-non-private-member-variables-in-classes)
  std::vector<QueryCondition>
      condition{};  // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes,
                    // misc-non-private-member-variables-in-classes)

public:
  using Ptr = std::unique_ptr<ComplexQuery>;
  /**
   * init a fast condition according to the table
   * note that the condition is only effective if the table fields are not
   * changed
   * @param table
   * @param conditions
   * @return a pair of the key and a flag
   * if flag is false, the condition is always false
   * in this situation, the condition may not be fully initialized to save time
   */
  [[nodiscard]] auto
  initCondition(const Table &table) -> std::pair<std::string, bool>;

  /**
   * skip the evaluation of KEY
   * (which should be done after initConditionFast is called)
   * @param conditions
   * @param object
   * @return
   */
  [[nodiscard]] auto evalCondition(const Table::Object &object) const -> bool;

  /**
   * This function seems have small effect and causes somme bugs
   * so it is not used actually
   * @param table
   * @param function
   * @return
   */
  [[nodiscard]] auto testKeyCondition(
      Table *table,
      const std::function<void(bool, Table::Object::Ptr &&)> &function) -> bool;

  ComplexQuery(std::string targetTable,
               std::vector<std::string>
                   operands,  // NOLINT(bugprone-easily-swappable-parameters)
               std::vector<QueryCondition> condition)
      : Query(std::move(targetTable)), operands(std::move(operands)),
        condition(std::move(condition)) {}

  /** Get operands in the query */
  [[nodiscard]] auto getOperands() const { return operands; }

  /** Get condition in the query, seems no use now */
  auto getCondition() { return condition; }
};

#endif  // SRC_QUERY_QUERY_H_
