#pragma once

/*
Copyright (c) 2016 - 2017, Roland Bock
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
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename L, typename R>
  struct divides_t
  {
    L l;
    R r;
  };

  template <typename L, typename R>
  constexpr auto operator/(L l, R r)
      -> std::enable_if_t<has_numeric_value_v<L> and has_numeric_value_v<R>, divides_t<L, R>>
  {
    return divides_t<L, R>{l, r};
  }

  template <typename L, typename R>
  struct value_type_of<divides_t<L, R>>
  {
    using type = numeric_t;
  };

  template <typename L, typename R>
  constexpr auto requires_braces_v<divides_t<L, R>> = true;

  template <typename DbConnection, typename L, typename R>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const divides_t<L, R>& t)
  {
    return to_sql_string(connection, embrace(t.l)) + " / " + to_sql_string(connection, embrace(t.r));
  }

  template <typename DbConnection, typename L1, typename R1, typename R2>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const divides_t<divides_t<L1, R1>, R2>& t)
  {
    return to_sql_string(connection, t.l) + " / " + to_sql_string(connection, embrace(t.r));
  }
}  // namespace sqlpp
