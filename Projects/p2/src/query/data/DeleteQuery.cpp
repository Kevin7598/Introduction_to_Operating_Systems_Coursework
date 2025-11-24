//
// Created by kaiqizhu on 25/10/23.
//

#include "DeleteQuery.h"

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../utils/formatter.h"
#include "../../utils/uexception.h"  // TableNameNotFound, IllFormedQueryCondition

#include "../../db/Database.h"
#include "../../db/Table.h"
#include "../QueryResult.h"

auto DeleteQuery::execute() -> QueryResult::Ptr {
  try {
    auto &database = Database::getInstance();
    auto *table = &database[this->targetTable];
    auto init = initCondition(*table);

    int count = 0;

    std::vector<Table::KeyType> keys_to_delete;
    for (auto it = table->begin(); it != table->end(); ++it) {
      if (evalCondition(*it)) {
        keys_to_delete.push_back(it->key());
      }
    }
    if (!keys_to_delete.empty()) {
      // Use batch erase for efficiency when removing many keys
      count = static_cast<int>(table->eraseMany(keys_to_delete));
    }

    return std::make_unique<RecordCountResult>(count);

  } catch (const TableNameNotFound &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "No such table.");
  } catch (const IllFormedQueryCondition &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable, e.what());
  } catch (const std::invalid_argument &e) {
    return std::make_unique<ErrorMsgResult>(qname, this->targetTable,
                                            "Unknown error '?'"_f % e.what());
  } catch (const std::exception &e) {
    return std::make_unique<ErrorMsgResult>(
        qname, this->targetTable, "Unknown error: " + std::string(e.what()));
  }
}
