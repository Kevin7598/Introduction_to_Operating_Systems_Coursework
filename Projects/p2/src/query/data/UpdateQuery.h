//
// Created by liu on 18-10-25.
//

#ifndef SRC_QUERY_DATA_UPDATEQUERY_H_
#define SRC_QUERY_DATA_UPDATEQUERY_H_

#include <string>

#include "../../db/Table.h"
#include "../Query.h"

class UpdateQuery : public ComplexQuery {
  static constexpr const char *qname = "UPDATE";
  Table::ValueType fieldValue = 0;  // (operands[0]=="KEY") ? 0 : parsed
  Table::FieldIndex fieldId = 0;
  Table::KeyType keyValue;

public:
  using ComplexQuery::ComplexQuery;

  auto execute() -> QueryResult::Ptr override;

  auto toString() -> std::string override;
};

#endif  // SRC_QUERY_DATA_UPDATEQUERY_H_
