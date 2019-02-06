#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp17/sqlite3/context.h>
#include <sqlpp17/sqlite3/value_type_to_sql_string.h>

namespace sqlpp::sqlite3::detail
{
  template <typename TableSpec, typename ColumnSpec>
  [[nodiscard]] auto to_sql_column_spec_string(sqlite3::context_t& context,
                                               [[maybe_unused]] const TableSpec&,
                                               const ColumnSpec& columnSpec)
  {
    auto ret = to_sql_name(context, columnSpec) + value_type_to_sql_string(context, type_t<typename ColumnSpec::value_type>{});

    if constexpr (not ColumnSpec::can_be_null)
    {
      ret += " NOT NULL";
    }

    if constexpr (std::is_same_v<std::decay_t<decltype(columnSpec.default_value)>, ::sqlpp::none_t>)
    {
    }
    else if constexpr (std::is_same_v<std::decay_t<decltype(columnSpec.default_value)>, ::sqlpp::auto_increment_t>)
    {
      static_assert(not ColumnSpec::can_be_null, "auto increment columns must not be null");
      static_assert(std::is_integral_v<typename ColumnSpec::value_type>, "auto increment columns must be integer");
      static_assert(std::is_same_v<typename TableSpec::primary_key, ColumnSpec>,
                    "auto increment columns must be integer primary key");
      ret += " PRIMARY KEY AUTOINCREMENT";
    }
    else
    {
      ret += " DEFAULT " + to_sql_string(context, columnSpec.default_value);
    }

    return ret;
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_create_columns_string(sqlite3::context_t& context,
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

    return (std ::string{} + ... +
            (separator.to_string() + to_sql_column_spec_string(context, TableSpec{}, ColumnSpecs{})));
  }

  template <typename TableSpec>
  [[nodiscard]] auto to_sql_primary_key(sqlite3::context_t& context, const ::sqlpp::table_t<TableSpec>& t)
  {
    using _primary_key = typename TableSpec::primary_key;
    if constexpr (std::is_same_v<_primary_key, ::sqlpp::none_t>)
    {
      return "";
    }
    else if constexpr (std::is_same_v<std::decay_t<decltype(_primary_key::default_value)>, ::sqlpp::auto_increment_t>)
    {
      return "";  // auto incremented primary keys need to be specified inline
    }
    else
    {
      return ", PRIMARY KEY (" + to_sql_name(context, _primary_key{}) + " ASC)";
    }
  }
}  // namespace sqlpp::sqlite3::detail

namespace sqlpp
{
  template <typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(sqlite3::context_t& context, const clause_base<create_table_t<Table>, Statement>& t)
  {
    auto ret = std::string{"CREATE TABLE "} + to_sql_string(context, t._table);
    ret += "(";
    ret += ::sqlpp::sqlite3::detail::to_sql_create_columns_string(context, column_tuple_of(t._table));
    ret += ::sqlpp::sqlite3::detail::to_sql_primary_key(context, t._table);
    ret += ")";

    return ret;
  }
}  // namespace sqlpp
