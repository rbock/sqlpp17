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
#include <vector>

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/first.h>
#include <sqlpp17/exception.h>
#include <sqlpp17/free_column.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct insert_values
    {
    };
  }  // namespace clause

  template <typename... Assignments>
  struct insert_values_t
  {
    std::tuple<Assignments...> _assignments;
  };

  template <typename... Assignments>
  struct nodes_of<insert_values_t<Assignments...>>
  {
    using type = type_vector<Assignments...>;
  };

  template <typename... Assignments>
  constexpr auto clause_tag<insert_values_t<Assignments...>> = clause::insert_values{};

  template <typename Statement, typename... Assignments>
  class clause_base<insert_values_t<Assignments...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<insert_values_t<Assignments...>, OtherStatement>& s) : _assignments(s._assignments)
    {
    }

    clause_base(const insert_values_t<Assignments...>& f) : _assignments(f._assignments)
    {
    }

    std::tuple<Assignments...> _assignments;
  };

  template <typename DbConnection, typename Statement, typename... Assignments>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection,
                                   const clause_base<insert_values_t<Assignments...>, Statement>& t)
  {
    auto ret = std::string{};
    // columns
    {
      ret += " (";
      ret += tuple_to_sql_string(connection, ", ",
                                 std::tuple(free_column_t{std::get<Assignments>(t._assignments).column}...));
      ret += ")";
    }

    // values
    {
      ret += " VALUES (";
      ret += tuple_to_sql_string(connection, ", ", std::tie(std::get<Assignments>(t._assignments).value...));
      ret += ")";
    }

    return ret;
  }

  struct insert_default_values_t
  {
  };

  template <>
  constexpr auto clause_tag<insert_default_values_t> = clause::insert_values{};

  template <typename Statement>
  class clause_base<insert_default_values_t, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<insert_default_values_t, OtherStatement>& s)
    {
    }

    clause_base(const insert_default_values_t& f)
    {
    }
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_default_values_require_all_defaults,
                              "default_values() requires all columns to have a default value");

  template <typename Db, typename Statement>
  constexpr auto check_clause_preparable(const type_t<clause_base<insert_default_values_t, Statement>>&)
  {
    using _table_t = typename Statement::into_table_t;
    constexpr auto _all_columns = columns_of_v<_table_t>;
    constexpr auto _default_columns = default_columns_of_v<_table_t>;

    if constexpr (_all_columns == _default_columns)
    {
      return succeeded{};
    }
    else
    {
      return failed<assert_default_values_require_all_defaults>{};
    }
  }

  template <typename DbConnection, typename Statement>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection,
                                   const clause_base<insert_default_values_t, Statement>& t)
  {
    return std::string{" DEFAULT VALUES"};
  }

  template <typename... Assignments>
  struct insert_multi_values_t
  {
    std::vector<std::tuple<Assignments...>> _rows;
  };

  template <typename... Assignments>
  struct nodes_of<insert_multi_values_t<Assignments...>>
  {
    using type = type_vector<Assignments...>;
  };

  template <typename... Assignments>
  constexpr auto clause_tag<insert_multi_values_t<Assignments...>> = clause::insert_values{};

  template <typename Statement, typename... Assignments>
  class clause_base<insert_multi_values_t<Assignments...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<insert_multi_values_t<Assignments...>, OtherStatement>& s) : _rows(s._rows)
    {
    }

    clause_base(const insert_multi_values_t<Assignments...>& f) : _rows(f._rows)
    {
    }

    std::vector<std::tuple<Assignments...>> _rows;
  };

  template <typename... Assignments>
  constexpr auto is_result_clause_v<insert_multi_values_t<Assignments...>> = true;

  template <typename... Assignments, typename Statement>
  class result_base<insert_multi_values_t<Assignments...>, Statement>
  {
  public:
    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      if (static_cast<const insert_multi_values_t<Assignments...>&>(Statement::of(this))._rows.empty())
      {
        return decltype(connection.insert(Statement::of(this))){};
      }
      else
      {
        return connection.insert(Statement::of(this));
      }
    }

    template <typename Connection>
    [[nodiscard]] auto _prepare(Connection& connection) const
    {
      if (static_cast<const insert_multi_values_t<Assignments...>&>(Statement::of(this))._rows.empty())
      {
        throw ::sqlpp::exception("Cannot prepare zero-line insert");
      }
      else
      {
        return connection.prepare_insert(Statement::of(this));
      }
    }
  };

  // this function assumes that there is something to do
  // the check if there is at least one row has to be performed elsewhere
  template <typename DbConnection, typename Statement, typename... Assignments>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection,
                                   const clause_base<insert_multi_values_t<Assignments...>, Statement>& t)
  {
    auto ret = std::string{};

    // columns
    {
      ret += " (";
      ret += tuple_to_sql_string(connection, ", ",
                                 std::tuple(free_column_t{std::get<Assignments>(t._assignments).column}...));
      ret += ")";
    }

    // values
    {
      ret += " VALUES (";
      auto first = true;
      for (const auto& row : t._rows)
      {
        if (!first)
          ret += ", ";
        first = false;
        ret += "(";
        ret += tuple_to_sql_string(connection, ", ", std::tie(std::get<Assignments>(t._assignments).value...));
        ret += ")";
      }
    }

    return ret;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_at_least_one_arg, "at least one assignment required in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_are_assignments,
                              "at least one argument is not an assignment in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_contain_no_duplicates,
                              "at least one duplicate column detected in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_assignments_are_allowed,
                              "at least one assignment is prohibited by its column definition in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_args_affect_single_table,
                              "set() arguments contain assignments from more than one table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_insert_set_is_not_missing_assignment,
                              "at least one required column is missing in set()");

  template <typename... Assignments>
  constexpr auto check_insert_set_args()
  {
    if constexpr (sizeof...(Assignments) == 0)
    {
      return failed<assert_insert_set_at_least_one_arg>{};
    }
    else if constexpr (!(true && ... && is_assignment_v<Assignments>))
    {
      return failed<assert_insert_set_args_are_assignments>{};
    }
    else if constexpr (type_set<char_sequence_of_t<column_of_t<Assignments>>...>().size() != sizeof...(Assignments))
    {
      return failed<assert_insert_set_args_contain_no_duplicates>{};
    }
    else if constexpr ((false || ... || is_read_only_v<column_of_t<Assignments>>))
    {
      return failed<assert_insert_set_assignments_are_allowed>{};
    }
    else if constexpr (type_set<table_spec_of_t<column_of_t<Assignments>>...>().size() != 1)
    {
      return failed<assert_insert_set_args_affect_single_table>{};
    }
    else if constexpr (not(type_set<column_of_t<Assignments>...>() >=
                           required_insert_columns_of_v<table_spec_of_t<column_of_t<detail::first_t<Assignments...>>>>))
    {
      return failed<assert_insert_set_is_not_missing_assignment>{};
    }
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
      return Statement::replace_clause(this, insert_default_values_t{});
    }

    template <typename... Assignments>
    [[nodiscard]] constexpr auto set(Assignments... assignments) const
    {
      constexpr auto check = check_insert_set_args<remove_optional_t<Assignments>...>();
      if constexpr (check)
      {
        using row_t = std::tuple<Assignments...>;
        return Statement::replace_clause(this, insert_values_t<Assignments...>{row_t{assignments...}});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }

    template <typename... Assignments>
    [[nodiscard]] constexpr auto multiset(std::vector<std::tuple<remove_optional_t<Assignments>...>> assignments) const
    {
      constexpr auto check = check_insert_set_args<remove_optional_t<Assignments>...>();
      if constexpr (check)
      {
        return Statement::replace_clause(this, insert_multi_values_t<Assignments...>{assignments});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }
  };

  template <typename DbConnection, typename Statement>
  decltype(auto) operator<<(const DbConnection& connection, const clause_base<no_insert_values_t, Statement>&)
  {
    return connection;
  }
}  // namespace sqlpp
