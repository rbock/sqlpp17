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
    struct group_by
    {
    };
  }  // namespace clause

  template <typename... Columns>
  struct group_by_t
  {
    std::tuple<Columns...> _columns;
  };

  template <typename... Columns>
  struct nodes_of<group_by_t<Columns...>>
  {
    using type = type_vector<Columns...>;
  };

  template <typename... Columns>
  struct provided_aggregates_of<group_by_t<Columns...>>
  {
    static constexpr auto value = type_set<Columns...>();
  };

  template <typename Table>
  constexpr auto clause_tag<group_by_t<Table>> = clause::group_by{};

  template <typename... Columns, typename Statement>
  class clause_base<group_by_t<Columns...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<group_by_t<Columns...>, OtherStatement>& s) : _columns(s._columns)
    {
    }

    clause_base(const group_by_t<Columns...>& f) : _columns(f._columns)
    {
    }

    std::tuple<Columns...> _columns;
  };

  template <typename Context, typename... Columns, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<group_by_t<Columns...>, Statement>& t)
  {
    return std::string{" GROUP BY "} + tuple_to_sql_string(context, ", ", std::tie(std::get<Columns>(t._columns)...));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_not_empty, "group_by() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_are_expressions,
                              "group_by() args must be value expressions (e.g. columns)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_group_by_args_contain_no_aggregate,
                              "group_by() args must not contain aggregate expressions (e.g. max or count)");

  template <typename... T>
  constexpr auto check_group_by_arg(const T&...)
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_group_by_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_expression_v<T>))
    {
      return failed<assert_group_by_args_are_expressions>{};
    }
    else if constexpr (recursive_contains_aggregate<type_set_t<>>(type_vector<T...>()))
    {
      return failed<assert_group_by_args_contain_no_aggregate>{};
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
      constexpr auto _check = check_group_by_arg(columns...);
      if constexpr (_check)
      {
        return new_statement(*this, group_by_t<Columns...>{std::tuple(columns...)});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto group_by(std::tuple<Columns...> columns) const
    {
      constexpr auto _check = check_group_by_arg(std::declval<Columns>()...);
      if constexpr (_check)
      {
        return new_statement(*this, group_by_t<Columns...>{columns});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_group_by_t, Statement>& t)
  {
    return std::string{};
  }

  template <typename... Columns>
  [[nodiscard]] constexpr auto group_by(Columns&&... columns)
  {
    return statement<no_group_by_t>{}.group_by(std::forward<Columns>(columns)...);
  }
}  // namespace sqlpp
