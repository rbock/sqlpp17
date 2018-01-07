#pragma once

/*
Copyright (c) 2018 - 2018, Roland Bock
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
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  namespace clause
  {
    struct truncate
    {
    };
  }  // namespace clause

  template <typename Table>
  struct truncate_t
  {
  };

  template <typename Table>
  struct nodes_of<truncate_t<Table>>
  {
    using type = type_vector<Table>;
  };

  template <typename Table>
  constexpr auto clause_tag<truncate_t<Table>> = clause::truncate{};

  template <typename Table, typename Statement>
  class clause_base<truncate_t<Table>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<truncate_t<Table>, OtherStatement>& t) : _table(t.table)
    {
    }

    clause_base(Table table) : _table(table)
    {
    }

    Table _table;
  };

  template <typename Table>
  constexpr auto is_result_clause_v<truncate_t<Table>> = true;

  template <typename Table, typename Statement>
  class result_base<truncate_t<Table>, Statement>
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

  template <typename Context, typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<truncate_t<Table>, Statement>& t)
  {
    return "TRUNCATE " + to_sql_name(context, name_tag_of_t<Table>{});
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_truncate_arg_is_table, "truncate() arg has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_truncate_arg_no_read_only_table, "truncate() arg must not be read-only table");

  template <typename T>
  constexpr auto check_truncate_arg()
  {
    if constexpr (!is_table_v<T>)
    {
      return failed<assert_truncate_arg_is_table>{};
    }
    else if constexpr (is_read_only_v<T>)
    {
      return failed<assert_truncate_arg_no_read_only_table>{};
    }
    else
      return succeeded{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto truncate(Table table)
  {
    if constexpr (constexpr auto check = check_truncate_arg<Table>(); check)
    {
      return statement<truncate_t<Table>>{table};
    }
    else
    {
      return ::sqlpp::bad_expression_t{check};
    }
  }
}  // namespace sqlpp
