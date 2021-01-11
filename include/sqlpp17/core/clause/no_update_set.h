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

#include <sqlpp17/core/clause/update_set.h>
#include <sqlpp17/core/statement.h>

namespace sqlpp
{
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
    if constexpr (sizeof...(Assignments))
    {
      return failed<assert_update_set_at_least_one_arg>{};
    }
    else if constexpr (!(true && ... && is_assignment_v<Assignments>))
    {
      return failed<assert_update_set_args_are_assignments>{};
    }
    else if constexpr (type_set<char_sequence_of_t<Assignments>...>().size() != sizeof...(Assignments))
    {
      return failed<assert_update_set_args_contain_no_duplicates>{};
    }
    else if constexpr (!(true && ... && is_update_allowed_v<column_of_t<Assignments>>))
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
    [[nodiscard]] constexpr auto update_set(Assignments... assignments) const
    {
      constexpr auto _check = check_update_set_arg<Assignments...>();
      if constexpr (_check)
      {
        return new_statement(*this, update_set_t<Assignments...>{assignments...});
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
    return statement<no_update_set_t>{}.update_set(std::forward<Assignments>(assignments)...);
  }
}  // namespace sqlpp
