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

#include <sqlpp17/join/join.h>
#include <sqlpp17/join/join_rhs.h>
#include <sqlpp17/join/on.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/unconditional.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_on_is_boolean_expression, "argument is not a boolean expression in on()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_join_on_no_foreign_table_dependencies,
                              "on() condition must not depend on other tables");

  namespace detail
  {
    template <typename ConditionlessJoin, typename Expr>
    constexpr auto check_join(const ConditionlessJoin&, const Expr&)
    {
      if
        constexpr(!is_expression<Expr> || !is_boolean<Expr>)
        {
          return failed<assert_on_is_boolean_expression>{};
        }
      else if
        constexpr(!(required_tables_of<Expr> <= provided_tables_of<ConditionlessJoin>))
        {
          return failed<assert_join_on_no_foreign_table_dependencies>{};
        }
      else
        return succeeded{};
    }
  }

  template <typename JoinType, typename Lhs, typename Rhs>
  class conditionless_join_t
  {
    template <typename R, typename On>
    constexpr auto make_rhs(R r, On on) const
    {
      return join_rhs_t<JoinType, R, On>{r, on};
    }

    template <typename R, typename On>
    constexpr auto make_rhs(sqlpp::optional<R> r, On on) const
    {
      return make_optional(r.to_be_used, make_rhs(r.value, on));
    }

    template <typename L, typename R>
    constexpr auto make_join(L l, R r) const
    {
      return join_t<L, R>{l, r};
    }

  public:
    template <typename Expr>
    [[nodiscard]] auto on(const Expr& expr) const
    {
      constexpr auto check = check_join(*this, expr);
      if
        constexpr(check)
        {
          return make_join(_lhs, make_rhs(_rhs, on_t<Expr>{expr}));
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }

    [[nodiscard]] constexpr auto unconditionally() const
    {
      return make_join(_lhs, make_rhs(_rhs, on_t<unconditional_t>{}));
    }

    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename JoinType, typename Lhs, typename Rhs>
  constexpr auto is_conditionless_join_v<conditionless_join_t<JoinType, Lhs, Rhs>> = true;

  template <typename JoinType, typename Lhs, typename Rhs>
  constexpr auto provided_tables_of_v<conditionless_join_t<JoinType, Lhs, Rhs>> =
      provided_tables_of_v<Lhs> | provided_tables_of_v<Rhs>;
}

