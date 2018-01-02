#pragma once

/*
Copyright (c) 2017, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or
   other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string>

#include <sqlpp17/clause/create_table.h>
#include <sqlpp17/column.h>
#include <sqlpp17/data_types.h>
#include <sqlpp17/table.h>

namespace sqlpp::mysql
{
  class connection_t;
}

namespace sqlpp::mysql::detail
{
  template <typename ValueType>
  [[nodiscard]] auto value_type_to_sql_string(const ValueType&)
  {
    static_assert(wrong<ValueType>, "unknown value type for CREATE TABLE");
  }

  [[nodiscard]] inline auto value_type_to_sql_string(const int64_t&)
  {
    return " BIGINT";
  }

  template <uint8_t Size>
  [[nodiscard]] inline auto value_type_to_sql_string(const ::sqlpp::varchar<Size>&)
  {
    return " VARCHAR(" + std::to_string(Size) + ")";
  }

  template <typename ColumnSpec>
  [[nodiscard]] auto to_sql_column_spec_string(const mysql::connection_t& connection, const ColumnSpec& columnSpec)
  {
    auto ret = to_sql_name(connection, columnSpec) + value_type_to_sql_string(typename ColumnSpec::value_type{});

    if constexpr (!ColumnSpec::can_be_null)
    {
      ret += " NOT NULL";
    }

    if constexpr (std::is_same_v<std::decay_t<decltype(columnSpec.default_value)>, ::sqlpp::none_t>)
    {
    }
    else if constexpr (std::is_same_v<std::decay_t<decltype(columnSpec.default_value)>, ::sqlpp::auto_increment_t>)
    {
      ret += " AUTO_INCREMENT";
    }
    else
    {
#warning need to implement default values
      // ret += " DEFAULT=" + to_sql_string(connection, columnSpec.default_value);
    }

    return ret;
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_create_columns_string(const mysql::connection_t& connection,
                                                  const std::tuple<column_t<TableSpec, ColumnSpecs>...>& t)
  {
    struct
    {
      bool first = true;
      [[nodiscard]] auto to_string() -> std::string
      {
        if (first)
        {
          first = false;
          return "";
        }
        else
        {
          return ", ";
        }
      }
    } separator;

    return (std ::string{} + ... + (separator.to_string() + to_sql_column_spec_string(connection, ColumnSpecs{})));
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_primary_key(const mysql::connection_t& connection,
                                        const ::sqlpp::table_t<TableSpec, ColumnSpecs...>& t)
  {
    using _primary_key = typename TableSpec::primary_key;
    if constexpr (std::is_same_v<_primary_key, ::sqlpp::none_t>)
    {
      return "";
    }
    else
    {
      return ", PRIMARY KEY (" + to_sql_name(connection, _primary_key{}) + ")";
    }
  }
}  // namespace sqlpp::mysql::detail

namespace sqlpp
{
  template <typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(const mysql::connection_t& connection,
                                   const clause_base<create_table_t<Table>, Statement>& t)
  {
    auto ret = std::string{"CREATE TABLE "} + to_sql_string(connection, t._table);
    ret += "(";
    ret += ::sqlpp::mysql::detail::to_sql_create_columns_string(connection, column_tuple_of(t._table));
    ret += ::sqlpp::mysql::detail::to_sql_primary_key(connection, t._table);
    ret += ")";

    return ret;
  }
}  // namespace sqlpp
