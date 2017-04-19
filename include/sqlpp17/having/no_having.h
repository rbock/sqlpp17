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

#include <sqlpp17/having/having.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_having_arg_is_expression, "having() arg has to be a boolean expression");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_having_arg_is_boolean, "having() arg has to be a boolean expression");

  template <typename T>
  constexpr auto check_having_arg(const T&)
  {
    if
      constexpr(!is_expression_v<T>)
      {
        return failed<assert_having_arg_is_expression>{};
      }
    else if
      constexpr(!is_boolean_v<T>)
      {
        return failed<assert_having_arg_is_boolean>{};
      }
    else
      return succeeded{};
  }

  struct no_having_t
  {
  };

  template <typename Statement>
  class clause_base<no_having_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_having_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Condition>
    [[nodiscard]] constexpr auto having(Condition condition) const
    {
      constexpr auto check = check_having_arg(condition);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_having_t>(having_t<Condition>{condition});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_having_t, Statement>&)
  {
    return context;
  };

  template <typename Condition>
  [[nodiscard]] constexpr auto having(Condition&& condition)
  {
    return statement<no_having_t>{}.having(std::forward<Condition>(condition));
  }
}

