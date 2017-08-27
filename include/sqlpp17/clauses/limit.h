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
#include <sqlpp17/detail/unused.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_set.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct limit
    {
    };

    struct order_by;
  }

  template <typename Number>
  struct limit_t
  {
    Number _number;
  };

  template <typename Number>
  constexpr auto clause_tag<limit_t<Number>> = clause::limit{};

  template <typename Number, typename Statement>
  class clause_base<limit_t<Number>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<limit_t<Number>, OtherStatement>& s) : _number(s._number)
    {
    }

    clause_base(const limit_t<Number>& f) : _number(f._number)
    {
    }

    Number _number;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_limit_used_with_order_by, "limit must be used with order_by");

  template <typename Db, typename Number, typename... Clauses>
  constexpr auto check_clause_executable(const type_t<clause_base<limit_t<Number>, statement<Clauses...>>>& t)
  {
    constexpr auto _tag_set = type_set(clause_tag<Clauses>...);
    if constexpr (!_tag_set.template count<clause::order_by>())
    {
      return failed<assert_limit_used_with_order_by>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename Context, typename Number, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<limit_t<Number>, Statement>& t)
  {
    if (detail::unused(t._number))
      return context;

    context << " LIMIT " << de_optionalize(t._number);

    return context;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_limit_arg_is_integral_value, "limit() arg has to be an integral value");

  template <typename T>
  constexpr auto check_limit_arg()
  {
    if constexpr (!std::is_integral_v<T>)
    {
      return failed<assert_limit_arg_is_integral_value>{};
    }
    else
      return succeeded{};
  }

  struct no_limit_t
  {
  };

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
      constexpr auto check = check_limit_arg<remove_optional_t<Value>>();
      if constexpr (check)
      {
        return Statement::replace_clause(this, limit_t<Value>{value});
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
