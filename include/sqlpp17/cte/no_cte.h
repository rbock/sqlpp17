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

#include <sqlpp17/cte/cte.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_is_statement, "cte.as() arg must be an sql statement");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_has_result_row,
                              "cte.as() arg must have a result_row, e.g. a select or union");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_without_with, "cte.as() arg must not contain a WITH clause");

  template <typename AliasProvider, typename Statement>
  constexpr auto check_cte_as_arg()
  {
    if constexpr (!is_statement_v<Statement>)
    {
      return failed<assert_cte_as_arg_is_statement>{};
    }
    else if constexpr (!has_result_row_v<Statement>)
    {
      return failed<assert_cte_as_arg_has_result_row>{};
    }
    else if constexpr (not provided_ctes_of_v<Statement>.empty())
    {
      return failed<assert_cte_as_arg_without_with>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename AliasProvider>
  struct cte_alias_t
  {
  public:
    template <typename Statement>
    [[nodiscard]] constexpr auto as(Statement s) const
    {
      constexpr auto check = check_cte_as_arg<AliasProvider, Statement>();
      if constexpr (check)
      {
        return cte_t<flat_t, AliasProvider, Statement>{s};
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }
  };

  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_cte_arg_is_name_tag_or_similar,
      "cte() arg must be a named expression (e.g. column, table), or a column/table spec, or a name tag");

  template <typename Tag>
  constexpr auto check_cte_args()
  {
    if constexpr (std::is_same_v<name_tag_of_t<Tag>, none_t>)
    {
      return failed<assert_cte_arg_is_name_tag_or_similar>{};
    }
    else
      return succeeded{};
  }

  template <typename NamedTypeOrTag>
  [[nodiscard]] constexpr auto cte([[maybe_unused]] NamedTypeOrTag)
  {
    if constexpr (constexpr auto check = check_cte_args<NamedTypeOrTag>(); check)
    {
      return cte_alias_t<name_tag_of_t<NamedTypeOrTag>>{};
    }
    else
    {
      return ::sqlpp::bad_expression_t{check};
    }
  }
}  // namespace sqlpp
