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
#warning : Need to add CTE checks for cte
  /*
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte, "cte() args must be sql statements");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte, "cte() args have result rows (like select)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte, "cte() args must have compatible result rows");
  */

  template <typename Statement>
  constexpr auto check_cte(const Statement&...)
  {
#warning : Need to check whether this statement has a result row, for instance
    /*
    if
      constexpr(!(is_statement_v<LeftSelect> && is_statement_v<RightSelect>))
      {
        return failed<assert_cte>{};
      }
    else if
      constexpr(!(has_result_row_v<LeftSelect> && has_result_row_v<RightSelect>))
      {
        return failed<assert_cte>{};
      }
    else if
      constexpr(!result_rows_are_compatible_v<typename LeftSelect::result_row_t, typename RightSelect::result_row_t>)
      {
        return failed<assert_cte>{};
      }
    else
    */
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
      constexpr auto check = check_cte_statement(s);
      if
        constexpr(check)
        {
          return cte_t<AliasProvider, Statement>{s};
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename AliasProvider>
  [[nodiscard]] constexpr auto cte(AliasProvider alias)
  {
#warning : Check if Alias Provider actually is one.
    return cte_alias_t<AliasProvider>{};
  }
}

