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
#include <variant>
#include <sqlpp17/all.h>
#include <sqlpp17/selected_columns/selected_columns.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_args_not_empty,
                              "columns() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_args_are_selectable,
                              "columns() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_args_have_unique_names, "columns() args must have unique names");

  template <typename... T>
  constexpr auto check_selected_columns_arg(const T&...)
  {
    if
      constexpr(sizeof...(T) == 0)
      {
        return failed<assert_selected_columns_args_not_empty>{};
      }
    else if
      constexpr(!all<is_selectable_v<T>...>)
      {
        return failed<assert_selected_columns_args_are_selectable>{};
      }
    else if
      constexpr(type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
      {
        return failed<assert_selected_columns_args_have_unique_names>{};
      }
    else
      return succeeded{};
  }

  struct no_selected_columns_t
  {
  };

  template <>
  constexpr auto clause_tag<no_selected_columns_t> = clause::selected_columns{};

  template <typename Statement>
  class clause_base<no_selected_columns_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_selected_columns_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Fields>
    [[nodiscard]] constexpr auto columns(Fields... columns) const
    {
      constexpr auto check = check_selected_columns_arg(columns...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_selected_columns_t>(
              selected_columns_t<Fields...>{std::make_tuple(columns...)});
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }

    template <typename... Fields>
    [[nodiscard]] constexpr auto columns(std::tuple<Fields...> columns) const
    {
      constexpr auto check = check_selected_columns_arg(std::declval<Fields>()...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_selected_columns_t>(
              selected_columns_t<Fields...>{columns});
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }

    template <typename... Fields>
    [[nodiscard]] constexpr auto columns(std::vector<std::variant<Fields...>> columns) const
    {
      constexpr auto check = check_selected_columns_arg(std::declval<Fields>()...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_selected_columns_t>(
              selected_columns_t<std::vector<std::variant<Fields...>>>{columns});
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_selected_columns_t, Statement>& t)
  {
    return context;
  }

  template <typename... Fields>
  [[nodiscard]] constexpr auto selected_columns(Fields&&... columns)
  {
    return statement<no_selected_columns_t>{}.columns(std::forward<Fields>(columns)...);
  }
}

