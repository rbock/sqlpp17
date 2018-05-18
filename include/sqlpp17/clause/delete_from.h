#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
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

namespace sqlpp
{
  namespace clause
  {
    struct delete_from
    {
    };
  }  // namespace clause

  template <typename Table>
  struct delete_from_t
  {
  };

  template <typename Table>
  struct nodes_of<delete_from_t<Table>>
  {
    using type = type_vector<Table>;
  };

  template <typename Table>
  constexpr auto clause_tag<delete_from_t<Table>> = clause::delete_from{};

  SQLPP_WRAPPED_STATIC_ASSERT(assert_delete_from_arg_is_table, "delete_from() arg has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_delete_from_arg_no_join, "delete_from() arg must not depend on other tables");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_delete_from_arg_no_cte, "delete_from() arg must not depend on other tables");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_delete_from_arg_no_read_only_table,
                              "delete_from() arg must not be read-only table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_delete_from_arg_no_required_tables,
                              "delete_from() arg must not depend on other tables");

  template <typename T>
  constexpr auto check_delete_from_arg()
  {
    if constexpr (!is_table_v<T>)
    {
      return failed<assert_delete_from_arg_is_table>{};
    }
    else if constexpr (is_join_v<T>)
    {
      return failed<assert_delete_from_arg_no_join>{};
    }
    else if constexpr (is_cte_v<T>)
    {
      return failed<assert_delete_from_arg_no_cte>{};
    }
    else if constexpr (is_read_only_v<T>)
    {
      return failed<assert_delete_from_arg_no_read_only_table>{};
    }
    else if constexpr (!required_tables_of_v<T>.empty())
    {
      return failed<assert_delete_from_arg_no_required_tables>{};
    }
    else
      return succeeded{};
  }

  template <typename Table, typename Statement>
  class clause_base<delete_from_t<Table>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<delete_from_t<Table>, OtherStatement>& t) : _table(t._table)
    {
    }

    clause_base(Table table) : _table(table)
    {
    }

    Table _table;
  };

  template <typename Table>
  constexpr auto is_result_clause_v<delete_from_t<Table>> = true;

  template <typename Table, typename Statement>
  class result_base<delete_from_t<Table>, Statement>
  {
  protected:
    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.delete_from(Statement::of(*this));
    }
  };

  template <typename Context, typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<delete_from_t<Table>, Statement>& t)
  {
    return std::string{"DELETE FROM "} + to_sql_string(context, t._table);
  }

  template <typename Table>
  [[nodiscard]] constexpr auto delete_from(Table table)
  {
    if constexpr (constexpr auto check = check_delete_from_arg<Table>(); check)
    {
      return statement<delete_from_t<Table>>{table} << statement<no_where_t>{};
    }
    else
    {
      return bad_expression_t{check};
    }
  }
}  // namespace sqlpp
