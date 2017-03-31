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

#include <sqlpp17/remove_table/remove_table.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_remove_table_arg_is_table, "remove_table() arg has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_remove_table_arg_no_read_only_table,
                              "remove_table() arg must not be read-only table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_remove_table_arg_no_required_tables,
                              "remove_table() arg must not depend on other tables");

  template <typename T>
  constexpr auto check_remove_table_arg(const T&)
  {
    if
      constexpr(!is_table_v<T>)
      {
        return failed<assert_remove_table_arg_is_table>{};
      }
    else if
      constexpr(is_read_only_table_v<T>)
      {
        return failed<assert_remove_table_arg_no_read_only_table>{};
      }
    else if
      constexpr(!required_tables_of_v<T>.empty())
      {
        return failed<assert_remove_table_arg_no_required_tables>{};
      }
    else
      return succeeded{};
  }

  struct no_remove_table_t
  {
  };

  template <>
  constexpr auto clause_tag<no_remove_table_t> = clause::remove_table{};

  template <typename Statement>
  class clause_base<no_remove_table_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_remove_table_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Table>
    [[nodiscard]] constexpr auto remove_table(Table t) const
    {
      constexpr auto check = check_remove_table_arg(t);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_remove_table_t>(remove_table_t<Table>{t});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_remove_table_t, Statement>&)
  {
    return context;
  }

  template <typename Table>
  [[nodiscard]] constexpr auto remove_table(Table&& t)
  {
    return statement<no_remove_table_t>{}.remove_table(std::forward<Table>(t));
  }
}

