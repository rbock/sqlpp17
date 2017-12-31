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

#include <sqlpp17/cte/cte.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_is_statement, "cte.as() arg must be an sql statement");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_has_result_row,
                              "cte.as() arg must have a result_row, e.g. a select or union");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_as_arg_is_not_initially_self_referential,
                              "cte.as() arg must not be self referential in the first part");

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
    else if constexpr (required_cte_names_of_v<Statement>.template count<AliasProvider>() != 0)
    {
      return failed<assert_cte_as_arg_is_not_initially_self_referential>{};
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
        return cte_t<AliasProvider, Statement>{s};
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_arg_is_alias_provider, "cte() arg must be an alias provider");

  template <typename AliasProvider>
  constexpr auto check_cte_arg()
  {
#warning : Need to reactivate the check
    if constexpr (false and !is_alias_provider_v<AliasProvider>)
    {
      return failed<assert_cte_arg_is_alias_provider>{};
    }
    else
    {
      return succeeded{};
    }
  }
  template <typename AliasProvider>
  [[nodiscard]] constexpr auto cte(AliasProvider alias)
  {
    constexpr auto check = check_cte_arg<AliasProvider>();
    if constexpr (check)
    {
      return cte_alias_t<AliasProvider>{};
    }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }
}  // namespace sqlpp
