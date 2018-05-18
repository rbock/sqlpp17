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

#include <tuple>

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct order_by
    {
    };
  }  // namespace clause

  template <typename... Columns>
  struct order_by_t
  {
    std::tuple<Columns...> _columns;
  };
  template <typename... Columns>
  order_by_t(std::tuple<Columns...>)->order_by_t<Columns...>;

  template <typename... Columns>
  struct nodes_of<order_by_t<Columns...>>
  {
    using type = type_vector<Columns...>;
  };

  template <typename Table>
  constexpr auto clause_tag<order_by_t<Table>> = clause::order_by{};

  template <typename... Columns, typename Statement>
  class clause_base<order_by_t<Columns...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<order_by_t<Columns...>, OtherStatement>& s) : _columns(s._columns)
    {
    }

    clause_base(const order_by_t<Columns...>& f) : _columns(f._columns)
    {
    }

    std::tuple<Columns...> _columns;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_not_having_aggregates_without_group_by,
                              "order_by() args must not be aggregates if there is no group by");

  template <typename Db, typename... Columns, typename... Clauses>
  constexpr auto check_clause_preparable(const type_t<clause_base<order_by_t<Columns...>, statement<Clauses...>>>& t)
  {
    using known_aggregates_t = decltype((::sqlpp::type_set() | ... | provided_aggregates_of_v<Clauses>));

    if constexpr ((known_aggregates_t::empty() and ... and recursive_contains_aggregate<known_aggregates_t, Columns>()))
    {
      return failed<assert_order_by_args_not_having_aggregates_without_group_by>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename Context, typename... Columns, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<order_by_t<Columns...>, Statement>& t)
  {
    return std::string{" ORDER BY "} + tuple_to_sql_string(context, ", ", t._columns);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_not_empty, "order_by() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_are_sort_expressions,
                              "order_by() args must be sort expressions (tab.foo.asc())");

  template <typename... T>
  constexpr auto check_order_by_arg(const T&...)
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_order_by_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_sort_order_v<T>))
    {
      return failed<assert_order_by_args_are_sort_expressions>{};
    }
    else
      return succeeded{};
  }

  struct no_order_by_t
  {
  };

  template <typename Statement>
  class clause_base<no_order_by_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_order_by_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Expressions>
    [[nodiscard]] constexpr auto order_by(Expressions... expressions) const
    {
      if constexpr (constexpr auto check = check_order_by_arg(expressions...); check)
      {
        return Statement::replace_clause(*this, order_by_t{std::tuple{expressions...}});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }

    template <typename... Expressions>
    [[nodiscard]] constexpr auto order_by(std::tuple<Expressions...> expressions) const
    {
      if constexpr (constexpr auto check = check_order_by_arg(std::declval<Expressions>()...); check)
      {
        return Statement::replace_clause(*this, order_by_t{expressions});
      }
      else
      {
        return ::sqlpp::bad_expression_t(check);
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_order_by_t, Statement>& t)
  {
    return std::string{};
  }

  template <typename... Expressions>
  [[nodiscard]] constexpr auto order_by(Expressions&&... expressions)
  {
    return statement<no_order_by_t>{}.order_by(std::forward<Expressions>(expressions)...);
  }
}  // namespace sqlpp
