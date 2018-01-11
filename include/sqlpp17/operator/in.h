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

#include <type_traits>
#include <sqlpp17/to_sql_string.h>

namespace sqlpp
{
  template <typename L, typename... Args>
  struct in_t
  {
    L l;
    std::tuple<Args...> args;
  };

  template <typename L, typename... Args>
  struct nodes_of<in_t<L, Args...>>
  {
    using type = type_vector<L, Args...>;
  };

  template <typename L, typename... Args>
  constexpr auto in(L l, Args... args)
      -> std::enable_if_t<((sizeof...(Args) > 0) and ... and values_are_compatible_v<L, Args>), in_t<L, Args...>>
  {
    return in_t<L, Args...>{l, std::tuple{args...}};
  }

  template <typename L, typename... Args>
  struct value_type_of<in_t<L, Args...>>
  {
    using type = bool;
  };

  template <typename L, typename... Args>
  constexpr auto requires_braces_v<in_t<L, Args...>> = true;

  template <typename Context, typename L, typename... Args>
  [[nodiscard]] auto to_sql_string(Context& context, const in_t<L, Args...>& t)
  {
    if constexpr (sizeof...(Args) == 1)
    {
      return to_sql_string(context, embrace(t.l)) + " IN(" + to_sql_string(context, std::get<0>(t.args)) + ")";
    }
    else
    {
      return to_sql_string(context, embrace(t.l)) + " IN(" + tuple_to_sql_string(context, ", ", t.args) + ")";
    }
  }
}  // namespace sqlpp
