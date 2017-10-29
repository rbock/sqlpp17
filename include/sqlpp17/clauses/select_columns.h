#pragma once

/*
Copyright (c) 2017, Roland Bock
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
#include <sqlpp17/column_spec.h>
#include <sqlpp17/detail/select_column_printer.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct select_columns
    {
    };
  }

  template <typename... Columns>
  struct select_columns_t
  {
    std::tuple<Columns...> _columns;
  };

  template <typename... Columns>
  constexpr auto is_result_clause_v<select_columns_t<Columns...>> = true;

  template <typename... Columns>
  constexpr auto clause_tag<select_columns_t<Columns...>> = clause::select_columns{};

  template <typename... Columns, typename Statement>
  class clause_base<select_columns_t<Columns...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<select_columns_t<Columns...>, OtherStatement>& s) : _columns(s._columns)
    {
    }

    clause_base(const select_columns_t<Columns...>& f) : _columns(f._columns)
    {
    }

    std::tuple<Columns...> _columns;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_all_aggregates_or_none,
                              "columns need to be either all aggregates or all non-aggregates");

  template <typename Db, typename... Columns, typename... Clauses>
  constexpr auto check_clause_executable(
      const type_t<clause_base<select_columns_t<Columns...>, statement<Clauses...>>>& t)
  {
    using known_aggregates_t = decltype((::sqlpp::type_set() | ... | provided_aggregates_v<Clauses>));

    constexpr auto all_aggregates = (true && ... && recursive_is_aggregate<known_aggregates_t, Columns>());
    constexpr auto no_aggregates = (true && ... && recursive_is_non_aggregate<known_aggregates_t, Columns>());

    if constexpr (!(all_aggregates || !no_aggregates))
    {
      return failed<assert_selected_columns_all_aggregates_or_none>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename... Columns, typename Statement>
  class result_base<select_columns_t<Columns...>, Statement>
  {
  public:
    using result_row_t = result_row_t<make_column_spec_t<Statement, Columns>...>;

    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.select(Statement::of(this), result_row_t{});
    }
  };

  template <typename Context, typename... Columns, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<select_columns_t<Columns...>, Statement>& t)
  {
    auto print = detail::select_column_printer<Context>{context, ", "};
    context << ' ';
    (..., print(std::get<Columns>(t._columns)));
    return context;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_not_empty,
                              "select columns() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_are_selectable,
                              "select columns() args must be selectable (i.e. named expressions)");
  /*
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_have_unique_names,
                              "select columns() args must have unique names");
  */

  template <typename... T>
  constexpr auto check_select_columns_arg()
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_select_columns_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_selectable_v<T>))
    {
      return failed<assert_select_columns_args_are_selectable>{};
    }
#warning : The compiler would see this anyway
    /*
    else if constexpr (type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
    {
      return failed<assert_select_columns_args_have_unique_names>{};
    }
    */
    else
      return succeeded{};
  }

  struct no_select_columns_t
  {
  };

  template <typename Statement>
  class clause_base<no_select_columns_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_select_columns_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(Columns... columns) const
    {
      constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>();
      if constexpr (check)
      {
        return Statement::replace_clause(this, select_columns_t<Columns...>{std::tuple(columns...)});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(std::tuple<Columns...> columns) const
    {
      constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>();
      if constexpr (check)
      {
        return Statement::replace_clause(this, select_columns_t<Columns...>{columns});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_select_columns_t, Statement>& t)
  {
    return context;
  }

  template <typename... Columns>
  [[nodiscard]] constexpr auto select_columns(Columns&&... columns)
  {
    return statement<no_select_columns_t>{}.columns(std::forward<Columns>(columns)...);
  }
}
