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
#include <type_traits>
#include <sqlpp17/operator_fwd.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_valid_plus_operands, "invalid operands for operator plus");

  template <typename ValueType, typename L, typename R>
  struct plus_t
  {
    constexpr plus_t(ValueType, L left, R right) : l(left), r(right)
    {
    }

    L l;
    R r;
  };

  template <typename ValueTypeLeft, typename ValueTypeRight>
  constexpr auto check_plus(const ValueTypeLeft&, const ValueTypeRight&)
  {
    return failed<assert_valid_plus_operands>{};
  }

  template <typename L, typename R>
  constexpr auto operator+(L l, R r)
  {
    constexpr auto check = check_plus(value_type_of(l), value_type_of(r));
    if constexpr (check)
    {
      return plus_t{check.value_type, l, r};
    }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }

  template <typename ValueType, typename L, typename R>
  constexpr auto value_type_of_v<plus_t<ValueType, L, R>> = ValueType{};

  template <typename ValueType, typename L, typename R>
  constexpr auto requires_braces_v<plus_t<ValueType, L, R>> = true;

  template <typename Context, typename V, typename L, typename R>
  constexpr decltype(auto) operator<<(Context& context, const plus_t<V, L, R>& t)
  {
    return context << embrace(t.l) << " + " << embrace(t.r);
  }

  template <typename Context, typename V1, typename L1, typename R1, typename V2, typename R2>
  constexpr decltype(auto) operator<<(Context& context, const plus_t<V2, plus_t<V1, L1, R1>, R2>& t)
  {
    return context << t.l << " + " << embrace(t.r);
  }
}
