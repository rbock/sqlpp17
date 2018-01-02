#pragma once

/*
Copyright (c) 2016 - 2017, Roland Bock
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

#include <sqlpp17/clause/from.h>
#include <sqlpp17/clause/where.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  namespace clause
  {
    struct create_table
    {
    };
  }  // namespace clause

  template <typename Table>
  struct create_table_t
  {
  };

  template <typename Table>
  constexpr auto clause_tag<create_table_t<Table>> = clause::create_table{};

#warning : Need to check this before running
  /*
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_table_arg_is_table, "from_table() arg has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_table_arg_no_read_only_table, "from_table() arg must not be read-only table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_table_arg_no_required_columns,
                              "from_table() arg must not depend on other tables");

  template <typename T>
  constexpr auto check_from_table_arg(const T&)
  {
    if constexpr (!is_table_v<T>)
    {
      return failed<assert_from_table_arg_is_table>{};
    }
    else if constexpr (is_read_only_v<T>)
    {
      return failed<assert_from_table_arg_no_read_only_table>{};
    }
    else if constexpr (!required_columns_of_v<T>.empty())
    {
      return failed<assert_from_table_arg_no_required_columns>{};
    }
    else
      return succeeded{};
  }

  */
  template <typename Table, typename Statement>
  class clause_base<create_table_t<Table>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<create_table_t<Table>, OtherStatement>& t) : _table(t.table)
    {
    }

    clause_base(Table table) : _table(table)
    {
    }

    Table _table;
  };

  template <typename Table>
  constexpr auto is_result_clause_v<create_table_t<Table>> = true;

  template <typename Table, typename Statement>
  class result_base<create_table_t<Table>, Statement>
  {
  protected:
    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.execute(Statement::of(this));
    }

    template <typename Connection>
    [[nodiscard]] auto _prepare(Connection& connection) const
    {
      return prepared_statement_t{*this, connection.prepare_execute(Statement::of(this))};
    }
  };

  template <typename DbConnection, typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection,
                                   const clause_base<create_table_t<Table>, Statement>& t)
  {
    static_assert(wrong<DbConnection, clause_base<create_table_t<Table>, Statement>>,
                  "Missing specialization for to_sql_string() for the current connection type");
  }

  template <typename Table>
  [[nodiscard]] constexpr auto create_table(Table table)
  {
    return statement<create_table_t<Table>>{table};
  }
}  // namespace sqlpp
