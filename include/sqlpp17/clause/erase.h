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

namespace sqlpp
{
  namespace clause
  {
    struct erase
    {
    };
  }  // namespace clause

  struct erase_t
  {
  };

  template <>
  constexpr auto clause_tag<erase_t> = clause::erase{};

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
  template <typename Statement>
  class clause_base<erase_t, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<erase_t, OtherStatement>&)
    {
    }

    clause_base() = default;
  };

  template <>
  constexpr auto is_result_clause_v<erase_t> = true;

  template <typename Statement>
  class result_base<erase_t, Statement>
  {
  protected:
    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.erase(Statement::of(this));
    }
  };

  template <typename DbConnection, typename Statement>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const clause_base<erase_t, Statement>& t)
  {
    return std::string{"DELETE"};
  }

  [[nodiscard]] constexpr auto erase()
  {
    return statement<erase_t, no_from_t, no_where_t>{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto erase_from(Table&& table)
  {
    return erase().from(std::forward<Table>(table));
  }
}  // namespace sqlpp
