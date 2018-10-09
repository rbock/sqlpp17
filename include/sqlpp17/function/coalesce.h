#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <sqlpp17/bad_expression.h>
#include <sqlpp17/to_sql_string.h>
#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename Arg0, typename Arg1, typename... Args>
  struct coalesce_t
  {
    std::tuple<Arg0, Arg1, Args...> args;
  };

  template <typename Arg0, typename Arg1, typename... Args>
  struct nodes_of<coalesce_t<Arg0, Arg1, Args...>>
  {
    using type = type_vector<Arg0, Arg1, Args...>;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_coalesce_args_are_compatible,
                              "coalesce() args must be compatible (e.g. all args are numeric)");

  template <typename Arg0, typename Arg1, typename... Args>
  constexpr auto check_coalesce_args()
  {
    if constexpr (not(values_are_compatible_v<Arg0, Arg1> and ... and values_are_compatible_v<Arg0, Args>))
    {
      return failed<assert_coalesce_args_are_compatible>{};
    }
    else
      return succeeded{};
  }

  template <typename Arg0, typename Arg1, typename... Args, typename = check_for_expression<Arg0, Arg1, Args...>>
  constexpr auto coalesce(Arg0 arg0, Arg1 arg1, Args... args)
  {
    if constexpr (constexpr auto _check = check_coalesce_args<Arg0, Arg1, Args...>(); _check)
    {
      return coalesce_t<Arg0, Arg1, Args...>{std::tuple{arg0, arg1, args...}};
    }
    else
    {
      return bad_expression_t{_check};
    }
  }

  template <typename Arg0, typename Arg1, typename... Args>
  struct value_type_of<coalesce_t<Arg0, Arg1, Args...>>
  {
    using type = value_type_of_t<Arg0>;
  };

  template <typename Context, typename Arg0, typename Arg1, typename... Args>
  [[nodiscard]] auto to_sql_string(Context& context, const coalesce_t<Arg0, Arg1, Args...>& t)
  {
    return "COALESCE(" + tuple_to_sql_string(context, ", ", t.args) + ")";
  }

}  // namespace sqlpp
