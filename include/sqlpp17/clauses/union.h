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

#warning : Need result_row here
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/clauses/select.h>  // This is for select and result_row
#include <sqlpp17/flags.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct union_
    {
    };
  }

  template <typename Flag, typename LeftSelect, typename RightSelect>
  struct union_t
  {
    Flag _flag;
    LeftSelect _left;
    RightSelect _right;
  };

  template <typename Flag, typename LeftSelect, typename RightSelect>
  constexpr auto is_result_clause_v<union_t<Flag, LeftSelect, RightSelect>> = true;

  template <typename Flag, typename LeftSelect, typename RightSelect>
  constexpr auto clause_tag<union_t<Flag, LeftSelect, RightSelect>> = clause::union_{};

  template <typename Flag, typename LeftSelect, typename RightSelect, typename Statement>
  class clause_base<union_t<Flag, LeftSelect, RightSelect>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<union_t<Flag, LeftSelect, RightSelect>, OtherStatement>& s)
        : _left(s._left), _right(s._right)
    {
    }

    clause_base(const union_t<Flag, LeftSelect, RightSelect>& f) : _flag(f._flag), _left(f._left), _right(f._right)
    {
    }

    Flag _flag;
    LeftSelect _left;
    RightSelect _right;
  };

  template <typename LeftColumnSpec, typename RightColumnSpec>
  struct merge_column_spec
  {
    static_assert(column_specs_are_compatible_v<LeftColumnSpec, RightColumnSpec>);

    using _alias_t = typename LeftColumnSpec::_alias_t;
    using _value_t = value_type_of_t<LeftColumnSpec>;
    static constexpr auto _can_be_null_tag =
        tag_if_v<tag::can_be_null, (can_be_null_v<LeftColumnSpec> || can_be_null_v<RightColumnSpec>)>;
    static constexpr auto _null_is_trivial_value_tag =
        tag_if_v<tag::null_is_trivial_value,
                 (null_is_trivial_value_v<LeftColumnSpec> || null_is_trivial_value_v<RightColumnSpec>)>;

    using type = column_spec<_alias_t, _value_t, _can_be_null_tag | _null_is_trivial_value_tag>;
  };

  template <typename LeftResultRow, typename RightResultRow>
  struct merge_result_row_specs
  {
    static_assert(wrong<merge_result_row_specs>, "Invalid arguments for merge_result_row_specs");
  };

  template <typename... LeftColumnSpecs, typename... RightColumnSpecs>
  struct merge_result_row_specs<result_row_t<LeftColumnSpecs...>, result_row_t<RightColumnSpecs...>>
  {
    using type = result_row_t<typename merge_column_spec<LeftColumnSpecs, RightColumnSpecs>::type...>;
  };

  template <typename Flag, typename LeftSelect, typename RightSelect, typename Statement>
  class result_base<union_t<Flag, LeftSelect, RightSelect>, Statement>
  {
  public:
    using result_row_t =
        typename merge_result_row_specs<typename LeftSelect::result_row_t, typename RightSelect::result_row_t>::type;

    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.select(Statement::of(this), result_row_t{});
    }
  };

  template <typename Context, typename Flag, typename LeftSelect, typename RightSelect, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<union_t<Flag, LeftSelect, RightSelect>, Statement>& t)
  {
    return context << embrace(t._left) << " UNION " << embrace(t._right);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_are_statements, "union_() args must be sql statements");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_have_result_rows, "union_() args have result rows (like select)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_union_args_have_compatible_rows, "union_() args must have compatible result rows");

  template <typename LeftSelect, typename RightSelect>
  constexpr auto check_union_args(const LeftSelect& l, const RightSelect& r)
  {
    if constexpr (!(is_statement_v<LeftSelect> && is_statement_v<RightSelect>))
    {
      return failed<assert_union_args_are_statements>{};
    }
    else if constexpr (!(has_result_row_v<LeftSelect> && has_result_row_v<RightSelect>))
    {
      return failed<assert_union_args_have_result_rows>{};
    }
    else if constexpr (!result_rows_are_compatible_v<typename LeftSelect::result_row_t,
                                                     typename RightSelect::result_row_t>)
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
    if constexpr (check)
    {
      using u = union_t<all_t, LeftSelect, RightSelect>;
      return statement<u>{detail::statement_constructor_arg(u{all, l, r})};
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
    if constexpr (check)
    {
      using u = union_t<distinct_t, LeftSelect, RightSelect>;
      return statement<u>{detail::statement_constructor_arg(u{distinct, l, r})};
    }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }
}
