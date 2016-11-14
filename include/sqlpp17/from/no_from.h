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

#include <sqlpp17/from/from.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_arg_is_not_conditionless_join,
                              "from() arg must not be a conditionless join, use .on() or .unconditionally()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_arg_is_table, "from() arg has to be a table or join");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_arg_no_required_tables, "from() arg must not depend on other tables");

  template <typename T>
  constexpr auto check_from_arg(const T&)
  {
    if
      constexpr(is_conditionless_join_v<T>)
      {
        return failed<assert_from_arg_is_not_conditionless_join>{};
      }
    else if
      constexpr(!is_table_v<T>)
      {
        return failed<assert_from_arg_is_table>{};
      }
    else if
      constexpr(!required_tables_of<T>.empty())
      {
        return failed<assert_from_arg_no_required_tables>{};
      }
    else
      return succeeded{};
  }

  struct no_from_t
  {
  };

  template <typename Connection, typename Statement>
  class clause_base<no_from_t, Connection, Statement>
  {
  public:
    template <typename Table>
    [[nodiscard]] constexpr auto from(Table t) const
    {
      constexpr auto check = detail::check_from_arg(t);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_from_t>(from_t<Table>{{t}});
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }
  };

  template <typename Context, typename Connection, typename Statement>
  class interpreter_t<Context, clause_base<no_from_t, Connection, Statement>>
  {
    using T = clause_base<no_from_t, Connection, Statement>;

    static Context& _(const T&, Context& context)
    {
      return context;
    }
  };

  template <typename Table>
  [[nodiscard]] constexpr auto from(Table&& t)
  {
    return statement<void, no_from_t>{}.from(std::forward<Table>(t));
  }
}

