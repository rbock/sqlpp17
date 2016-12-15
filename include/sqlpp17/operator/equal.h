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

#include <type_traits>
#include <sqlpp17/operator_fwd.h>
#include <sqlpp17/is_trivial_value.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_valid_equal_operands, "invalid operands for operator equal");

  template <typename L, typename R>
  struct equal_t
  {
    L l;
    R r;
  };

  template <typename L, typename R, typename ValueTypeLeft, typename ValueTypeRight>
  constexpr auto operator_equal(L, R, const ValueTypeLeft&, const ValueTypeRight&)
  {
    return failed<assert_valid_equal_operands>{};
  }

  template <typename L, typename R>
  constexpr auto operator==(L l, R r)
  {
    auto op = operator_equal(l, r, value_type_of(l), value_type_of(r));
    if
      constexpr(!is_failed(op))
      {
        return op;
      }
    else
    {
      return ::sqlpp::bad_statement_t<std::decay_t<decltype(op)>>{};
    }
  }

  template <typename L, typename R>
  constexpr auto is_expression_v<equal_t<L, R>> = true;

  template <typename L, typename R>
  constexpr auto value_type_of_v<equal_t<L, R>> = boolean_t{};

  template <typename L, typename R>
  constexpr auto requires_braces_v<equal_t<L, R>> = true;

  template <typename Context, typename L, typename R>
  constexpr decltype(auto) operator<<(Context& context, const equal_t<L, R>& t)
  {
    if (null_is_trivial_value(t.l) and is_trivial_value(t.r))
    {
      return context << embrace(t.l) << " IS NULL ";
    }
    else
    {
      return context << embrace(t.l) << " = " << embrace(t.r);
    }
  }
}
