//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_QUERYRESULT_H_
#define SRC_QUERY_QUERYRESULT_H_

#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "../utils/formatter.h"  // NOLINT(misc-include-cleaner)

class QueryResult {
public:
  using Ptr = std::unique_ptr<QueryResult>;

  virtual auto success() -> bool = 0;

  virtual auto display() -> bool = 0;

  virtual ~QueryResult() = default;

  QueryResult() = default;
  QueryResult(const QueryResult &) = default;
  auto operator=(const QueryResult &) -> QueryResult & = default;
  QueryResult(QueryResult &&) = default;
  auto operator=(QueryResult &&) -> QueryResult & = default;

  friend auto operator<<(std::ostream &ostream,
                         const QueryResult &table) -> std::ostream &;

protected:
  virtual auto output(std::ostream &ostream) const -> std::ostream & = 0;
};

class FailedQueryResult : public QueryResult {
  std::string data;

public:
  auto success() -> bool override { return false; }

  auto display() -> bool override { return false; }
};

class SucceededQueryResult : public QueryResult {
public:
  auto success() -> bool override { return true; }
};

class NullQueryResult : public SucceededQueryResult {
public:
  auto display() -> bool override { return false; }

protected:
  auto output(std::ostream &ostream) const
      -> std::ostream
          & override {  // NOLINT(readability-convert-member-functions-to-static)
    return ostream;
  }
};

class ErrorMsgResult : public FailedQueryResult {
  std::string msg;

public:
  auto display() -> bool override { return false; }

  ErrorMsgResult(
      const char *qname,
      const std::string &msg) {  // NOLINT(bugprone-easily-swappable-parameters)
    this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
  }

  ErrorMsgResult(
      const char *qname,
      const std::string &table,  // NOLINT(bugprone-easily-swappable-parameters)
      const std::string &msg) {
    this->msg = R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
  }

protected:
  auto output(std::ostream &ostream) const -> std::ostream & override {
    return ostream << msg << "\n";
  }
};

class SuccessMsgResult : public SucceededQueryResult {
  std::string msg;

public:
  auto display() -> bool override { return false; }

  explicit SuccessMsgResult(const int number) {
    this->msg = R"(ANSWER = "?".)"_f % number;
    this->msg += "\n";
  }

  explicit SuccessMsgResult(const std::vector<int> &results) {
    std::stringstream sstream{};
    sstream << "ANSWER = ( ";
    for (auto result : results) {
      sstream << result << " ";
    }
    sstream << ")\n";
    this->msg = sstream.str();
  }

  explicit SuccessMsgResult(const char *qname) { (void)qname; }

  SuccessMsgResult(const char *qname, const std::string &msg) {
    (void)qname;
    (void)msg;
  }

  SuccessMsgResult(const std::string &table, const char *qname,
                   const std::string &msg) {
    (void)qname;
    (void)table;
    (void)msg;
  }

protected:
  auto output(std::ostream &ostream) const -> std::ostream & override {
    return ostream << msg;
  }
};

// A success result that contains a raw message and is intended to be shown
// directly (no "Query ... success" header). Useful for SELECT which wants
// to print only result rows.
class RawSuccessResult : public SucceededQueryResult {
  std::string msg;

public:
  auto display() -> bool override { return true; }

  explicit RawSuccessResult(std::string raw) : msg(std::move(raw)) {}

protected:
  auto output(std::ostream &ostream) const -> std::ostream & override {
    return ostream << msg << "\n";
  }
};

class RecordCountResult : public SucceededQueryResult {
  int affectedRows;

public:
  auto display() -> bool override { return true; }

  explicit RecordCountResult(int count) : affectedRows(count) {}

protected:
  auto output(std::ostream &ostream) const -> std::ostream & override {
    return ostream << "Affected ? rows."_f % affectedRows << "\n";
  }
};

#endif  // SRC_QUERY_QUERYRESULT_H_
