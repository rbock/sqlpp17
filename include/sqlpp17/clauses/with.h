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
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct with
    {
    };
  }

  enum class with_mode
  {
    flat,
    recursive
  };

  template <with_mode Mode, typename... CommonTableExpressions>
  struct with_t
  {
    std::tuple<CommonTableExpressions...> _ctes;
  };

  template <with_mode Mode, typename... CommonTableExpressions, typename Statement>
  class clause_base<with_t<Mode, CommonTableExpressions...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<with_t<Mode, CommonTableExpressions...>, OtherStatement>& s) : _ctes(s._ctes)
    {
    }

    clause_base(const with_t<Mode, CommonTableExpressions...>& f) : _ctes(f._ctes)
    {
    }

    std::tuple<CommonTableExpressions...> _ctes;
  };

  template <typename Context, with_mode Mode, typename... CommonTableExpressions, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<with_t<Mode, CommonTableExpressions...>, Statement>& t)
  {
#warning : WITH cannot occur after a compound operator like UNION
#warning : THis is not complete!
    return context << "WITH ";
  }

#warning : Need to add CTE checks for with
  /*
  SQLPP_WRAPPED_STATIC_ASSERT(assert_with_args_are_statements, "with() args must be sql statements");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_with_args_have_result_rows, "with() args have result rows (like select)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_with_args_have_compatible_rows, "with() args must have compatible result rows");
  */

  template <with_mode Mode, typename... CommonTableExpressions>
  constexpr auto check_with_args(const CommonTableExpressions&...)
  {
#warning : Need to check wether expressions are recursive if they shouldn't
    /*
    if
      constexpr(!(is_statement_v<LeftSelect> && is_statement_v<RightSelect>))
      {
        return failed<assert_with_args_are_statements>{};
      }
    else if
      constexpr(!(has_result_row_v<LeftSelect> && has_result_row_v<RightSelect>))
      {
        return failed<assert_with_args_have_result_rows>{};
      }
    else if
      constexpr(!result_rows_are_compatible_v<typename LeftSelect::result_row_t, typename RightSelect::result_row_t>)
      {
        return failed<assert_with_args_have_compatible_rows>{};
      }
    else
    */
    {
      return succeeded{};
    }
  }

  struct no_with_t
  {
  };

  template <typename Statement>
  class clause_base<no_with_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_with_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... CommonTableExpressions>
    [[nodiscard]] constexpr auto with(CommonTableExpressions... ctes) const
    {
      constexpr auto check = check_with_args<with_mode::flat>(ctes...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_with_t>(
              with_t<with_mode::flat, CommonTableExpressions...>{ctes...});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... CommonTableExpressions>
    [[nodiscard]] constexpr auto with_recursive(CommonTableExpressions... ctes) const
    {
      constexpr auto check = check_with_args<with_mode::recursive>(ctes...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_with_t>(
              with_t<with_mode::recursive, CommonTableExpressions...>{ctes...});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_with_t, Statement>&)
  {
    return context;
  }

  template <typename... CommonTableExpressions>
  [[nodiscard]] constexpr auto with(CommonTableExpressions&&... ctes)
  {
    return statement<no_with_t>{}.with(std::forward<CommonTableExpressions...>(ctes...));
  }

  template <typename... CommonTableExpressions>
  [[nodiscard]] constexpr auto with_recursive(CommonTableExpressions&&... ctes)
  {
    return statement<no_with_t>{}.with(std::forward<CommonTableExpressions...>(ctes...));
  }
}