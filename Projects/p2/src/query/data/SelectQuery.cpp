//
// Created by kaiqizhu on 25/10/23.
//

#include "SelectQuery.h"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"

#include "../../db/Database.h"
#include "../../db/Table.h"
#include "../QueryResult.h"

auto SelectQuery::execute() -> QueryResult::Ptr {
  try {
    auto &database = Database::getInstance();
    auto *table = &database[this->targetTable];
    auto init = initCondition(*table);

    std::vector<std::string> matches;
    std::vector<int> token;
    for (const auto &operand : operands) {
      if (operand == "KEY") {
        token.push_back(-1);
      } else {
        token.push_back(static_cast<int>(table->getFieldIndex(operand)));
      }
    }

    for (auto it = table->begin(); it != table->end(); ++it) {
      if (evalCondition(*it)) {
        std::string result = "( ";
        for (const auto &fieldIndex : token) {
          if (fieldIndex == -1) {
            result += it->key();
            result += " ";
          } else {
            result += std::to_string(
                (*it)[static_cast<Table::FieldIndex>(fieldIndex)]);
            result += " ";
          }
        }
        result += ")";
        matches.push_back(result);
      }
    }

    if (matches.empty()) {
      return std::make_unique<NullQueryResult>();
    }

    std::sort(matches.begin(), matches.end(),
              [](const auto &lhs, const auto &rhs) { return lhs < rhs; });

    std::stringstream stream;
    for (size_t idx = 0; idx < matches.size(); ++idx) {
      stream << matches[idx];
      if (idx != matches.size() - 1) {
        stream << "\n";
      }
    }

    return std::make_unique<RawSuccessResult>(stream.str());
  }

  catch (const TableNameNotFound &e) {
    const std::string result = "No such table.";
    return std::make_unique<ErrorMsgResult>(SelectQuery::qname,
                                            this->targetTable, result);
  } catch (const IllFormedQueryCondition &e) {
    return std::make_unique<ErrorMsgResult>(SelectQuery::qname,
                                            this->targetTable, e.what());
  } catch (const std::invalid_argument &e) {
    return std::make_unique<ErrorMsgResult>(SelectQuery::qname,
                                            this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(SelectQuery::qname,
                                            this->targetTable,
                                            "Unkonwn error '?'."_f % e.what());
  }
}
