#pragma once

/*
Copyright (c) 2017 - 2017, Roland Bock
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

#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_multi_column_args_not_empty,
                              "select columns() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_multi_column_args_are_selectable,
                              "select columns() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_multi_column_args_have_unique_names,
                              "select columns() args must have unique names");

  template <typename... T>
  constexpr auto check_multi_column_arg()
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_multi_column_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_selectable_v<T>))
    {
      return failed<assert_multi_column_args_are_selectable>{};
    }
    else if constexpr (type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
    {
      return failed<assert_multi_column_args_have_unique_names>{};
    }
    else
      return succeeded{};
  }

  template <typename... Columns>
  class multi_column_t
  {
  public:
    std::tuple<Columns...> _columns;

    constexpr multi_column_t(std::tuple<Columns...> columns) : _columns(columns)
    {
      if constexpr (constexpr auto check = check_multi_column_arg<remove_optional_t<Columns>...>(); not check)
      {
        ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename... Columns>
  struct nodes_of<multi_column_t<Columns...>>
  {
    using type = type_vector<Columns...>;
  };

  template <typename DbConnection, typename... Columns>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const multi_column_t<Columns...>& t)
  {
    return tuple_to_sql_string(connection, t._tuples);
  }
}  // namespace sqlpp
