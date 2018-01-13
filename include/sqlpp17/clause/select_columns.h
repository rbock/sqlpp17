#pragma once

/*
Copyright (c) 2016 - 2017, Roland Bock
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
#include <sqlpp17/multi_column.h>
#include <sqlpp17/result.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/to_sql_name.h>
#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct select_columns
    {
    };
  }  // namespace clause

  template <typename... Columns>
  struct select_columns_t
  {
    std::tuple<Columns...> _columns;
  };

  template <typename... Columns>
  struct nodes_of<select_columns_t<Columns...>>
  {
    using type = type_vector<Columns...>;
  };

  template <typename... Columns>
  constexpr auto clause_tag<select_columns_t<Columns...>> = clause::select_columns{};

  template <typename Column>
  struct select_column_t
  {
    constexpr select_column_t(Column column) : _column(column)
    {
    }

    constexpr select_column_t(const select_column_t&) = default;
    constexpr select_column_t(select_column_t&&) = default;
    select_column_t& operator=(const select_column_t&) = default;
    select_column_t& operator=(select_column_t&&) = default;
    ~select_column_t() = default;

    Column _column;
  };

  template <typename Context, typename Column>
  [[nodiscard]] auto to_sql_string(Context& context, const select_column_t<Column>& t)
  {
    return has_value(t._column)
               ? to_sql_string(context, get_value(t._column))
               : std::string("NULL AS ") + to_sql_name(context, name_tag_of_t<remove_optional_t<Column>>{});
  }

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

    std::tuple<select_column_t<Columns>...> _columns;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_all_aggregates_or_none,
                              "selected columns need to be either all aggregates or all non-aggregates");

  template <typename Db, typename... Columns, typename... Clauses>
  constexpr auto check_clause_preparable(
      const type_t<clause_base<select_columns_t<Columns...>, statement<Clauses...>>>& t)
  {
    using known_aggregates_t = decltype((::sqlpp::type_set() | ... | provided_aggregates_of_v<Clauses>));

    constexpr auto all_aggregates = (true and ... and recursive_is_aggregate<known_aggregates_t, Columns>());
    constexpr auto no_aggregates = not(false or ... or recursive_contains_aggregate<known_aggregates_t, Columns>());

    if constexpr (not(all_aggregates or no_aggregates))
    {
      return failed<assert_selected_columns_all_aggregates_or_none>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename... Columns>
  constexpr auto is_result_clause_v<select_columns_t<Columns...>> = true;

  template <typename... Columns, typename Statement>
  class result_base<select_columns_t<Columns...>, Statement>
  {
  protected:
    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      using _result_handle_t = decltype(connection.select(Statement::of(this)));
      if constexpr (has_result_row_v<_result_handle_t>)
      {
        return connection.select(Statement::of(this));
      }
      else
      {
        return ::sqlpp::result_t<result_row_of_t<result_base>, _result_handle_t>{
            connection.select(Statement::of(this))};
      }
    }

    template <typename Connection>
    [[nodiscard]] auto _prepare(Connection& connection) const
    {
      return prepared_statement_t{*this, connection.prepare_select(Statement::of(this))};
    }

  public:
    [[nodiscard]] constexpr auto get_no_of_result_columns() const
    {
      return sizeof...(Columns);
    }
  };

  template <typename... Columns, typename Statement>
  struct result_row_of<result_base<select_columns_t<Columns...>, Statement>>
  {
    using type = result_row_t<make_column_spec_t<Statement, Columns>...>;
  };

  template <typename Context, typename... Columns, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<select_columns_t<Columns...>, Statement>& t)
  {
    return " " + tuple_to_sql_string(context, ", ", t._columns);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_not_empty,
                              "select columns() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_are_selectable,
                              "select columns() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_have_unique_names,
                              "select columns() args must have unique names");

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
    else if constexpr (type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
    {
      return failed<assert_select_columns_args_have_unique_names>{};
    }
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
      if constexpr (constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>(); check)
      {
        return Statement::replace_clause(this, select_columns_t<Columns...>{std::tuple(columns...)});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(std::tuple<Columns...> columns) const
    {
      if constexpr (constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>(); check)
      {
        return Statement::replace_clause(this, select_columns_t<Columns...>{columns});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(multi_column_t<Columns...> columns) const
    {
      if constexpr (constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>(); check)
      {
        return Statement::replace_clause(this, select_columns_t<Columns...>{columns._columns});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_select_columns_t, Statement>& t)
  {
    return std::string{};
  }

  template <typename... Columns>
  [[nodiscard]] constexpr auto select_columns(Columns&&... columns)
  {
    return statement<no_select_columns_t>{}.columns(std::forward<Columns>(columns)...);
  }
}  // namespace sqlpp
