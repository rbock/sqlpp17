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
#include <sqlpp17/group_by/group_by.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_not_empty, "group_by() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_are_selectable,
                              "group_by() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_have_unique_names, "group_by() args must have unique names");

  template <typename... T>
  constexpr auto check_group_by_arg(const T&...)
  {
    if
      constexpr(sizeof...(T) == 0)
      {
        return failed<assert_group_by_args_not_empty>{};
      }
    else if
      constexpr(!(true && ... && is_selectable_v<T>))
      {
        return failed<assert_group_by_args_are_selectable>{};
      }
    else if
      constexpr(type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
      {
        return failed<assert_group_by_args_have_unique_names>{};
      }
    else
      return succeeded{};
  }

  struct no_group_by_t
  {
  };

  template <typename Statement>
  class clause_base<no_group_by_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_group_by_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Columns>
    [[nodiscard]] constexpr auto group_by(Columns... columns) const
    {
      constexpr auto check = check_group_by_arg(columns...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_group_by_t>(
              group_by_t<Columns...>{std::make_tuple(columns...)});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto group_by(std::tuple<Columns...> columns) const
    {
      constexpr auto check = check_group_by_arg(std::declval<Columns>()...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_group_by_t>(group_by_t<Columns...>{columns});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_group_by_t, Statement>& t)
  {
    return context;
  }

  template <typename... Columns>
  [[nodiscard]] constexpr auto group_by(Columns&&... columns)
  {
    return statement<no_group_by_t>{}.group_by(std::forward<Columns>(columns)...);
  }
}

