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
  template <typename L>
  struct is_null_t
  {
    L l;
  };

  template <typename L>
  constexpr auto is_null(L l) -> std::enable_if_t<has_boolean_value_v<L>, is_null_t<L>>
  {
    return is_null_t<L>{l};
  }

  template <typename L>
  struct value_type_of<is_null_t<L>>
  {
    using type = bool;
  };

  template <typename L>
  constexpr auto requires_braces_v<is_null_t<L>> = true;

  template <typename DbConnection, typename L>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const is_null_t<L>& t)
  {
    return to_sql_string(embrace(t.l)) + " IS NULL";
  }
}  // namespace sqlpp
