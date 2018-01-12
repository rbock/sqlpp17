#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp17/to_sql_string.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename L, typename R>
  struct assign_t
  {
    L column;
    R value;
  };

  template <typename L, typename R>
  struct nodes_of<assign_t<L, R>>
  {
    using type = type_vector<L, R>;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_assign_null_to_nullable_columns_only,
                              "NULL must not be assigned to columns that cannot be NULL");

  SQLPP_WRAPPED_STATIC_ASSERT(assert_assign_optional_to_nullable_columns_only,
                              "optional values must not be assigned to columns that cannot be NULL");

  template <typename L, typename R>
  constexpr auto check_assign_args()
  {
    if constexpr (not can_be_null_v<L> and std::is_same_v<::std::nullopt_t, R>)
    {
      return failed<assert_assign_null_to_nullable_columns_only>{};
    }
    else if constexpr (not can_be_null_v<L> and is_optional_v<R>)
    {
      return failed<assert_assign_optional_to_nullable_columns_only>{};
    }
    else
      return succeeded{};
  }

  template <typename L, typename R>
  constexpr auto assign(L column, R value) -> std::enable_if_t<values_are_compatible_v<L, R>, assign_t<L, R>>
  {
    if constexpr (constexpr auto check = check_assign_args<L, R>())
    {
      return assign_t<L, R>{column, value};
    }
    else
    {
      return bad_expression_t{check};
    }
  }

  template <typename L, typename R>
  constexpr auto is_assignment_v<assign_t<L, R>> = true;

  template <typename L, typename R>
  struct column_of<assign_t<L, R>>
  {
    using type = L;
  };

  template <typename L, typename R>
  constexpr auto requires_braces_v<assign_t<L, R>> = true;

  template <typename Context, typename L, typename R>
  [[nodiscard]] auto to_sql_string(Context& context, const assign_t<L, R>& t)
  {
    return to_sql_string(context, t.column) + " = " + to_sql_string(context, embrace(t.value));
  }
}  // namespace sqlpp
