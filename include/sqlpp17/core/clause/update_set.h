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

#include <sqlpp17/core/clause_fwd.h>
#include <sqlpp17/core/free_column.h>
#include <sqlpp17/core/statement.h>
#include <sqlpp17/core/tuple_to_sql_string.h>
#include <sqlpp17/core/type_traits.h>
#include <sqlpp17/core/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename Assignment>
  struct update_assignment_t
  {
    Assignment _assignment;
  };

  template <typename Context, typename Assignment>
  [[nodiscard]] auto to_sql_string(Context& context, const update_assignment_t<Assignment>& assignment)
  {
    auto ret = to_sql_string(context, free_column_t<column_of_t<remove_optional_t<Assignment>>>{});
    if constexpr (::sqlpp::is_optional_v<Assignment>)
    {
      if (assignment._assignment)
        return ret + " = " + to_sql_string(context, assignment._assignment.value().value);
      else
      {
        return ret + " = " + ret;
      }
    }
    else
    {
      return ret + " = " + to_sql_string(context, assignment._assignment.value);
    }
  }
}  // namespace sqlpp

namespace sqlpp
{
  template <typename... Assignments>
  struct update_set_t
  {
    std::tuple<Assignments...> _assignments;
  };

  template <typename... Assignments>
  struct nodes_of<update_set_t<Assignments...>>
  {
    using type = type_vector<Assignments...>;
  };

  template <typename... Assignments>
  constexpr auto clause_tag<update_set_t<Assignments...>> = ::std::string_view{"update_set"};

  template <typename... Assignments, typename Statement>
  class clause_base<update_set_t<Assignments...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<update_set_t<Assignments...>, OtherStatement>& s) : _assignments(s._assignments)
    {
    }

    clause_base(const update_set_t<Assignments...>& f) : _assignments(f._assignments)
    {
    }

    std::tuple<Assignments...> _assignments;
  };

  template <typename... Assignments>
  constexpr auto is_result_clause_v<update_set_t<Assignments...>> = true;

  template <typename... Assignments>
  struct clause_result_type<update_set_t<Assignments...>>
  {
    using type = update_result;
  };

  template <typename Context, typename... Assignments, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<update_set_t<Assignments...>, Statement>& t)
  {
    return std::string{" SET "} +
           tuple_to_sql_string(context, ", ",
                               std::tuple(update_assignment_t<Assignments>{std::get<Assignments>(t._assignments)}...));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_update_set_at_least_one_arg, "at least one assignment required in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_update_set_args_are_assignments,
                              "at least one argument is not an assignment in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_update_set_args_contain_no_duplicates,
                              "at least one duplicate column detected in set()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_update_set_assignments_are_allowed,
                              "at least one assignment is prohibited by its column definition in set()");

  template <typename... Assignments>
  constexpr auto check_update_set_arg()
  {
    if constexpr (sizeof...(Assignments) == 0)
    {
      return failed<assert_update_set_at_least_one_arg>{};
    }
    else if constexpr (!(true && ... && is_assignment_v<remove_optional_t<Assignments>>))
    {
      return failed<assert_update_set_args_are_assignments>{};
    }
    else if constexpr (type_set<char_sequence_of_t<column_of_t<remove_optional_t<Assignments>>>...>().size() !=
                       sizeof...(Assignments))
    {
      return failed<assert_update_set_args_contain_no_duplicates>{};
    }
    else if constexpr ((false || ... || is_read_only_v<column_of_t<remove_optional_t<Assignments>>>))
    {
      return failed<assert_update_set_assignments_are_allowed>{};
    }
    else
      return succeeded{};
  }

  struct no_update_set_t
  {
  };

  template <typename Statement>
  class clause_base<no_update_set_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_update_set_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Assignments>
    [[nodiscard]] constexpr auto set(Assignments... assignments) const
    {
      constexpr auto _check = check_update_set_arg<Assignments...>();
      if constexpr (_check)
      {
        return new_statement(*this, update_set_t<Assignments...>{std::tuple{assignments...}});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_update_set_t, Statement>&)
  {
    return std::string{};
  }

  template <typename... Assignments>
  [[nodiscard]] constexpr auto update_set(Assignments&&... assignments)
  {
    return statement<no_update_set_t>{}.set(std::forward<Assignments>(assignments)...);
  }
}  // namespace sqlpp
