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

#include <sqlpp17/postgresql/context.h>
#include <sqlpp17/postgresql/value_type_to_sql_string.h>

namespace sqlpp::postgresql::detail
{
  template <typename ColumnSpec>
  [[nodiscard]] auto to_sql_column_spec_string(postgresql::context_t& context, const ColumnSpec& columnSpec)
  {
    auto ret = to_sql_name(context, columnSpec);

    if constexpr (ColumnSpec::has_auto_increment)
    {
      if constexpr (std::is_same_v<typename ColumnSpec::value_type, std::int16_t>)
      {
        ret += " SMALLSERIAL";
      }
      else if constexpr (std::is_same_v<typename ColumnSpec::value_type, std::int32_t>)
      {
        ret += " SERIAL";
      }
      else if constexpr (std::is_same_v<typename ColumnSpec::value_type, std::int64_t>)
      {
        ret += " BIGSERIAL";
      }
      else
      {
        static_assert(::sqlpp::wrong<ColumnSpec>, "Unexpected type for auto increment");
      }
    }
    else
    {
      ret += value_type_to_sql_string(context, type_t<typename ColumnSpec::value_type>{});

      if constexpr (!ColumnSpec::can_be_null)
      {
        ret += " NOT NULL";
      }

      if constexpr (ColumnSpec::has_default_value)
      {
        ret += " DEFAULT " + to_sql_string(context, columnSpec.default_value);
      }
    }

    return ret;
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_create_columns_string(postgresql::context_t& context,
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

    return (std ::string{} + ... + (separator.to_string() + to_sql_column_spec_string(context, ColumnSpecs{})));
  }

  template <typename TableSpec>
  [[nodiscard]] auto to_sql_primary_key(postgresql::context_t& context, const ::sqlpp::table_t<TableSpec>& t)
  {
    using _primary_key = typename TableSpec::primary_key;
    if constexpr (std::is_same_v<_primary_key, ::sqlpp::none_t>)
    {
      return "";
    }
    else
    {
      return ", PRIMARY KEY (" + to_sql_name(context, _primary_key{}) + ")";
    }
  }
}  // namespace sqlpp::postgresql::detail

namespace sqlpp
{
  template <typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(postgresql::context_t& context,
                                   const clause_base<create_table_t<Table>, Statement>& t)
  {
    auto ret = std::string{"CREATE TABLE "} + to_sql_string(context, t._table);
    ret += "(";
    ret += ::sqlpp::postgresql::detail::to_sql_create_columns_string(context, column_tuple_of(t._table));
    ret += ::sqlpp::postgresql::detail::to_sql_primary_key(context, t._table);
    ret += ")";

    return ret;
  }
}  // namespace sqlpp
