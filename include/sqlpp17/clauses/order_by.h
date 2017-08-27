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

#include <tuple>

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/list_printer.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct order_by
    {
    };
  }

  template <typename... Columns>
  struct order_by_t
  {
    std::tuple<Columns...> _columns;
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

  template <typename Context, typename... Columns, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<order_by_t<Columns...>, Statement>& t)
  {
    auto separate = detail::list_printer<Context>{context, ", "};
    context << ' ';
    (..., print(std::get<Columns>(t._columns)));
    return context;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_not_empty, "order_by() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_are_selectable,
                              "order_by() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_order_by_args_have_unique_names, "order_by() args must have unique names");

  template <typename... T>
  constexpr auto check_order_by_arg(const T&...)
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_order_by_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_selectable_v<T>))
    {
      return failed<assert_order_by_args_are_selectable>{};
    }
    else if constexpr (type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
    {
      return failed<assert_order_by_args_have_unique_names>{};
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
      constexpr auto check = check_order_by_arg(expressions...);
      if constexpr (check)
      {
        return Statement::of(this).template replace_clause<no_order_by_t>(order_by_t{std::make_tuple(expressions...)});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Expressions>
    [[nodiscard]] constexpr auto order_by(std::tuple<Expressions...> expressions) const
    {
      constexpr auto check = check_order_by_arg(std::declval<Expressions>()...);
      if constexpr (check)
      {
        return Statement::of(this).template replace_clause<no_order_by_t>(order_by_t{expressions});
      }
      else
      {
        return ::sqlpp::bad_statement_t(check);
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_order_by_t, Statement>& t)
  {
    return context;
  }

  template <typename... Expressions>
  [[nodiscard]] constexpr auto order_by(Expressions&&... expressions)
  {
    return statement<no_order_by_t>{}.order_by(std::forward<Expressions>(expressions)...);
  }
}
