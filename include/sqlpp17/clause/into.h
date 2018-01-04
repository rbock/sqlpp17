#pragma once

/*
Copyright (c) 2016, Roland Bock
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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/prepared_statement.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct into
    {
    };
  }  // namespace clause

  template <typename Table>
  struct into_t
  {
    Table _table;
  };

  template <typename Table>
  struct nodes_of<into_t<Table>>
  {
    using type = type_vector<Table>;
  };

  template <typename Table>
  constexpr auto clause_tag<into_t<Table>> = clause::into{};

  template <typename Table, typename Statement>
  class clause_base<into_t<Table>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<into_t<Table>, OtherStatement>& s) : _table(s._table)
    {
    }

    clause_base(const into_t<Table>& f) : _table(f._table)
    {
    }

    Table _table;
  };

  template <typename DbConnection, typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const clause_base<into_t<Table>, Statement>& t)
  {
    return std::string(" INTO ") + to_sql_string(connection, t._table);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_into_arg_is_table, "into() arg has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_into_arg_no_read_only_table, "into() arg must not be read-only table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_into_arg_no_required_tables, "into() arg must not depend on other tables");

  template <typename T>
  constexpr auto check_into_arg(const T&)
  {
    if constexpr (!is_table_v<T>)
    {
      return failed<assert_into_arg_is_table>{};
    }
    else if constexpr (is_read_only_v<T>)
    {
      return failed<assert_into_arg_no_read_only_table>{};
    }
    else if constexpr (!required_tables_of_v<T>.empty())
    {
      return failed<assert_into_arg_no_required_tables>{};
    }
    else
      return succeeded{};
  }

  struct no_into_t
  {
  };

  template <typename Statement>
  class clause_base<no_into_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_into_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Table>
    [[nodiscard]] constexpr auto into(Table t) const
    {
      constexpr auto check = check_into_arg(t);
      if constexpr (check)
      {
        return Statement::replace_clause(this, into_t<Table>{t});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Table>
  constexpr auto is_result_clause_v<into_t<Table>> = true;

  template <typename Table, typename Statement>
  class result_base<into_t<Table>, Statement>
  {
  public:
    using into_table_t = Table;

    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.insert(Statement::of(this));
    }

    template <typename Connection>
    [[nodiscard]] auto _prepare(Connection& connection) const
    {
      return prepared_statement_t{*this, connection.prepare_insert(Statement::of(this))};
    }
  };

  template <typename DbConnection, typename Statement>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const clause_base<no_into_t, Statement>&)
  {
    return std::string{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto into(Table&& t)
  {
    return statement<no_into_t>{}.into(std::forward<Table>(t));
  }
}  // namespace sqlpp
