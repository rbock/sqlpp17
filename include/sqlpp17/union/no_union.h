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

#include <sqlpp17/flags.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/union/union.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_are_statements, "union_() args must be sql statements");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_have_result_rows, "union_() args have result rows (like select)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_have_compatible_rows, "union_() args must have compatible result rows");

  template <typename LeftSelect, typename RightSelect>
  constexpr auto check_union_args(const LeftSelect& l, const RightSelect& r)
  {
    if
      constexpr(!(is_statement_v<LeftSelect> && is_statement_v<RightSelect>))
      {
        return failed<assert_union_args_are_statements>{};
      }
    else if
      constexpr(!(has_result_row_v<LeftSelect> && has_result_row_v<RightSelect>))
      {
        return failed<assert_union_args_have_result_rows>{};
      }
    else if
      constexpr(!result_rows_are_compatible_v<typename LeftSelect::result_row_t, typename RightSelect::result_row_t>)
      {
        return failed<assert_union_args_have_compatible_rows>{};
      }
    else
    {
      return succeeded{};
    }
  }

  struct no_union_t
  {
  };

  template <typename Statement>
  class clause_base<no_union_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_union_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename RightSelect>
    [[nodiscard]] constexpr auto union_all(RightSelect rhs) const
    {
      return union_all(Statement::of(this), rhs);
    }

    template <typename RightSelect>
    [[nodiscard]] constexpr auto union_distinct(RightSelect rhs) const
    {
      return union_all(Statement::of(this), rhs);
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_union_t, Statement>&)
  {
    return context;
  }

  template <typename LeftSelect, typename RightSelect>
  [[nodiscard]] constexpr auto union_all(LeftSelect l, RightSelect r)
  {
    constexpr auto check = check_union_args(l, r);
    if
      constexpr(check)
      {
        return statement<no_union_t>{}.template replace_clause<no_union_t>(
            union_t<all_t, LeftSelect, RightSelect>{all, l, r});
      }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }

  template <typename LeftSelect, typename RightSelect>
  [[nodiscard]] constexpr auto union_distinct(LeftSelect l, RightSelect r)
  {
    constexpr auto check = check_union_args(l, r);
    if
      constexpr(check)
      {
        return statement<no_union_t>{}.template replace_clause<no_union_t>(
            union_t<distinct_t, LeftSelect, RightSelect>{distinct, l, r});
      }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }
}

