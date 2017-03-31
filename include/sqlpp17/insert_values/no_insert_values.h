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

#include <vector>
#include <sqlpp17/insert_values/insert_column_values.h>
#include <sqlpp17/insert_values/insert_columns.h>
#include <sqlpp17/insert_values/insert_default_values.h>
#include <sqlpp17/insert_values/insert_values.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_are_assignments,
                              "at least one argument is not an assignment in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_contain_no_duplicates,
                              "at least one duplicate column detected in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_assignments_are_allowed,
                              "at least one assignment is prohibited by its column definition in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_affect_single_table,
                              "set() arguments contain assignments from more than one table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_at_least_one_arg, "at least one assignment required in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_no_missing_assignment,
                              "at least one required column is missing in set()");

  template <typename... T>
  constexpr auto check_insert_set_args(const T&...)
  {
    if
      constexpr(all<is_assignment_v<T>...>)
      {
        return failed<assert_insert_set_args_are_assignments>{};
      }
#warning missing conditions
    else
      return succeeded{};
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_args_are_columns,
                              "at least one argument is not an assignment in columns()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_args_contain_no_duplicates,
                              "at least one duplicate column detected in columns()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_are_allowed,
                              "at least one column is prohibited by its definition in columns()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_args_affect_single_table,
                              "columns() arguments contain columns from more than one table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_at_least_one_arg, "at least one column required in columns()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_columns_no_missing_column,
                              "at least one required column is missing in columns()");

  template <typename... T>
  constexpr auto check_insert_columns_args(const T&...)
  {
    if
      constexpr(all<is_column_v<T>...>)
      {
        return failed<assert_insert_columns_args_are_columns>{};
      }
#warning missing conditions
    else
      return succeeded{};
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_args_are_assignments,
                              "at least one argument is not an assignment in column_values()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_args_contain_no_duplicates,
                              "at least one duplicate column detected in column_values()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_are_allowed,
                              "at least one column is prohibited by its definition in column_values()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_args_affect_single_table,
                              "column_values() arguments contain columns from more than one table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_at_least_one_arg,
                              "at least one column required in column_values()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_column_values_no_missing_column,
                              "at least one required column is missing in column_values()");

  template <typename... Assignments>
  constexpr auto check_insert_column_values_args(const type_vector<Assignments...>&)
  {
    if
      constexpr(all<is_assignment_v<Assignments>...>)
      {
        return failed<assert_insert_column_values_args_are_assignments>{};
      }
#warning missing conditions
    else
      return succeeded{};
  }

  struct no_insert_values_t
  {
  };

  template <typename Statement>
  class clause_base<no_insert_values_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_insert_values_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    [[nodiscard]] constexpr auto default_values() const
    {
      return Statement::of(this).template replace_clause<no_insert_values_t>(insert_default_values_t{});
    }

    template <typename... Assignments>
    [[nodiscard]] constexpr auto set(Assignments... assignments) const
    {
      constexpr auto check = check_insert_set_args(assignments...);
      if
        constexpr(check)
        {
          using row_t = std::tuple<Assignments...>;
          return Statement::of(this).template replace_clause<no_insert_values_t>(
              insert_values_t<row_t>{row_t{assignments...}});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    // This one accepts multiple rows of assignments via values
    //   - a vector of identical types
    //   - several calls to values with a single row each. This could also contain parameters
    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(Columns... cols) const
    {
      constexpr auto check = check_insert_columns_args(cols...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_insert_values_t>(insert_columns_t<Columns...>{cols...});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Assignments>
    [[nodiscard]] constexpr auto column_values(std::vector<std::tuple<Assignments...>> assignments) const
    {
      constexpr auto check = check_insert_column_values_args(type_vector<Assignments...>{});
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_insert_values_t>(
              insert_column_values_t<Assignments...>{assignments});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_insert_values_t, Statement>&)
  {
    return context;
  }
}

