#pragma once

/*
Copyright (c) 2017, Roland Bock
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

#include <sqlpp17/operator.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename Expression>
  struct expr_t
  {
    template <typename Pattern>
    [[nodiscard]] constexpr auto like(Pattern pattern) const
    {
      return ::sqlpp::like(_expr, pattern);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto in(Exprs... exprs) const
    {
      return ::sqlpp::in(_expr, exprs...);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto not_in(Exprs... exprs) const
    {
      return ::sqlpp::not_in(_expr, exprs...);
    }

    [[nodiscard]] constexpr auto is_null() const
    {
      return ::sqlpp::is_null(_expr);
    }

    [[nodiscard]] constexpr auto is_not_null() const
    {
      return ::sqlpp::is_not_null(_expr);
    }

    [[nodiscard]] constexpr auto asc() const
    {
      return ::sqlpp::asc(_expr);
    }

    [[nodiscard]] constexpr auto desc() const
    {
      return ::sqlpp::desc(_expr);
    }

    template <typename Alias>
    [[nodiscard]] constexpr auto as(const Alias& alias) const
    {
      return ::sqlpp::as(_expr, alias);
    }

    Expression _expr;
  };

  template <typename Expression>
  struct nodes_of<expr_t<Expression>>
  {
    using type = type_vector<Expression>;
  };

  template <typename Expression>
  struct value_type_of<expr_t<Expression>>
  {
    using type = value_type_of_t<Expression>;
  };

  template <typename DbConnection, typename Expression>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const expr_t<Expression>& t)
  {
    return to_sql_string(connection, t._expr);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_expr_arg_is_expression, "expr() arg must be a value expression");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_expr_arg_is_not_alias, "expr() first arg must not be an alias");

  template <typename Expr>
  constexpr auto check_expr_args()
  {
    if constexpr (not is_expression_v<Expr>)
    {
      return failed<assert_expr_arg_is_expression>{};
    }
    else if constexpr (is_alias_v<Expr>)
    {
      return failed<assert_expr_arg_is_not_alias>{};
    }
    else
      return succeeded{};
  }

  template <typename Expression>
  [[nodiscard]] auto expr(Expression expr)
  {
    if constexpr (constexpr auto check = check_expr_args<Expression>(); check)
    {
      return expr_t<Expression>{expr};
    }
    else
    {
      return ::sqlpp::bad_expression_t{check};
    }
  }

}  // namespace sqlpp
