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

#include <type_traits>
#include <sqlpp17/alias.h>
#include <sqlpp17/to_sql_string.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_first_as_arg_is_expression, "as() first arg must be a value expression");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_first_as_arg_is_not_alias, "as() first arg must not be an alias");
  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_second_as_arg_is_name_tag_or_similar,
      "as() second arg must be a named expression (e.g. column, table), or a column/table spec, or a name tag");

  template <typename Expr, typename Tag>
  constexpr auto check_as_args()
  {
    if constexpr (not is_expression_v<Expr>)
    {
      return failed<assert_first_as_arg_is_expression>{};
    }
    else if constexpr (is_alias_v<Expr>)
    {
      return failed<assert_first_as_arg_is_not_alias>{};
    }
    else if constexpr (std::is_same_v<name_tag_of_t<Tag>, none_t>)
    {
      return failed<assert_second_as_arg_is_name_tag_or_similar>{};
    }
    else
      return succeeded{};
  }

  template <typename Expr, typename NamedTypeOrTag>
  [[nodiscard]] constexpr auto as(Expr expr, const NamedTypeOrTag& tag)
  {
    if constexpr (constexpr auto check = check_as_args<Expr, NamedTypeOrTag>(); check)
    {
      return alias_t<Expr, name_tag_of_t<NamedTypeOrTag>>{expr};
    }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }

}  // namespace sqlpp
