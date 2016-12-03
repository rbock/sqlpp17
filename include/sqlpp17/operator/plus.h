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
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_valid_plus_operands, "invalid operands for operator plus");

  template <typename ValueType, typename L, typename R>
  struct plus_t
  {
    L l;
    R r;
  };

  template <typename L, typename R, typename ValueTypeLeft, typename ValueTypeRight>
  constexpr auto operator_plus(L, R, ValueTypeRight, ValueTypeRight)
  {
    return failed<assert_valid_plus_operands>{};
  }

  struct integral_t
  {
  };

  template <typename L, typename R>
  constexpr auto operator_plus(L l, R r, integral_t, integral_t)
  {
    return plus_t<integral_t, L, R>{l, r};
  }

  template <typename L, typename R>
  constexpr auto operator+(L l, R r)
  {
    constexpr auto op = operator_plus(l, r, value_type_of(l), value_type_of(r));
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
}
