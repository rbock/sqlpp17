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

#include <sqlpp17/limit/limit.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_limit_arg_is_integral_value, "limit() arg has to be an integral value");

  template <typename T>
  constexpr auto check_limit_arg(const T&)
  {
    if
      constexpr(!std::is_integral_v<remove_optional_t<T>>)
      {
        return failed<assert_limit_arg_is_integral_value>{};
      }
    else
      return succeeded{};
  }

  struct no_limit_t
  {
  };

  template <>
  constexpr auto clause_tag<no_limit_t> = clause::limit{};

  template <typename Statement>
  class clause_base<no_limit_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_limit_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Value>
    [[nodiscard]] constexpr auto limit(Value value) const
    {
      constexpr auto check = check_limit_arg(value);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_limit_t>(limit_t<Value>{value});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_limit_t, Statement>&)
  {
    return context;
  }

  template <typename Value>
  [[nodiscard]] constexpr auto limit(Value&& value)
  {
    return statement<no_limit_t>{}.limit(std::forward<Value>(value));
  }
}

