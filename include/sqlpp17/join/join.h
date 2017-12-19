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

#include <sqlpp17/join/join_functions.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  class join_t : public join_functions<join_t<Lhs, JoinType, Rhs, Condition>>
  {
  public:
    constexpr join_t(Lhs lhs, JoinType, Rhs rhs, Condition condition) : _lhs(lhs), _rhs(rhs), _condition(condition)
    {
    }

    Lhs _lhs;
    Rhs _rhs;
    Condition _condition;
  };

  template <typename DbConnection, typename Lhs, typename JoinType, typename Rhs, typename Condition>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const join_t<Lhs, JoinType, Rhs, Condition>& t)
  {
    auto ret = to_sql_string(connection, t._lhs);

    if (has_value(t._rhs))
    {
      ret += JoinType::_name;
      ret += " JOIN ";
      ret += to_sql_string(connection, get_value(t._rhs));
      ret += to_sql_string(connection, t._condition);
    }

    return ret;
  }

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  constexpr auto is_join_v<join_t<Lhs, JoinType, Rhs, Condition>> = true;

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  constexpr auto is_table_v<join_t<Lhs, JoinType, Rhs, Condition>> = true;

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  constexpr auto columns_of_v<join_t<Lhs, JoinType, Rhs, Condition>> = columns_of_v<Lhs> | columns_of_v<Rhs>;

  template <typename Lhs, typename JoinType, typename Rhs, typename Condition>
  constexpr auto can_be_null_columns_of_v<join_t<Lhs, JoinType, Rhs, Condition>> =
      JoinType::template _can_be_null_columns_of<Lhs, Rhs>;

}  // namespace sqlpp
