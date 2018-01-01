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
  enum class sort_order
  {
    asc,
    desc,
  };

  template <typename L>
  struct sort_order_t
  {
    L l;
    sort_order order;
  };

  template <typename L>
  constexpr auto asc(L l) -> std::enable_if_t<has_boolean_value_v<L>, sort_order_t<L>>
  {
    return sort_order_t<L>{l, sort_order::asc};
  }

  template <typename L>
  constexpr auto requires_braces_v<sort_order_t<L>> = false;

  template <typename L>
  constexpr auto is_sort_order_v<sort_order_t<L>> = true;

  template <typename DbConnection>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const sort_order& t)
  {
    switch (t)
    {
      case sort_order::asc:
        return std::string("ASC");
      case sort_order::desc:
        return std::string("DESC");
    }
  }

  template <typename DbConnection, typename L>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const sort_order_t<L>& t)
  {
    return to_sql_string(embrace(t.l)) + to_sql_string(connection, t.order);
  }

}  // namespace sqlpp
