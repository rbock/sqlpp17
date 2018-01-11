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
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename Arg0, typename Arg1, typename... Args>
  struct concat_t
  {
    std::tuple<Arg0, Arg1, Args...> args;
  };

  template <typename Arg0, typename Arg1, typename... Args>
  struct nodes_of<concat_t<Arg0, Arg1, Args...>>
  {
    using type = type_vector<Arg0, Arg1, Args...>;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_concat_args_are_text, "concat() args must be text");

  template <typename Arg0, typename Arg1, typename... Args>
  constexpr auto check_concat_args()
  {
    if constexpr (not(has_text_value_v<Arg0> and (has_text_value_v<Arg1> and ... and has_text_value_v<Args>)))
    {
      return failed<assert_concat_args_are_text>{};
    }
    else
      return succeeded{};
  }

  template <typename Arg0, typename Arg1, typename... Args, typename = check_for_expression<Arg0, Arg1, Args...>>
  constexpr auto concat(Arg0 arg0, Arg1 arg1, Args... args)
  {
    if constexpr (constexpr auto check = check_concat_args<Arg0, Arg1, Args...>(); check)
    {
      return concat_t<Arg0, Arg1, Args...>{std::tuple{arg0, arg1, args...}};
    }
    else
    {
      return bad_expression_t{check};
    }
  }

  template <typename Arg0, typename Arg1, typename... Args>
  struct value_type_of<concat_t<Arg0, Arg1, Args...>>
  {
    using type = text_t;
  };

  template <typename Arg0, typename Arg1, typename... Args>
  struct requires_braces<concat_t<Arg0, Arg1, Args...>> : std::true_type
  {
  };

  template <typename Context, typename Arg0, typename Arg1, typename... Args>
  [[nodiscard]] auto to_sql_string(Context& context, const concat_t<Arg0, Arg1, Args...>& t)
  {
    return tuple_to_sql_string(context, " || ", t.args);
  }

}  // namespace sqlpp
