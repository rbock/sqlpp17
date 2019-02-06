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

#include <sqlpp17/to_sql_string.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/value_type_to_sql_string.h>

namespace sqlpp
{
  template <typename ValueType, typename Expression>
  struct sql_cast_t
  {
    Expression _expression;
  };

  template <typename ValueType, typename Expression>
  struct nodes_of<sql_cast_t<ValueType, Expression>>
  {
    using type = type_vector<Expression>;
  };

  template <typename ValueType, typename Expression>
  struct value_type_of<sql_cast_t<ValueType, Expression>>
  {
    using type = ValueType;
  };

  template <typename ValueType, typename Expression>
  struct name_tag_of<sql_cast_t<ValueType, Expression>>
  {
    using type = name_tag_of_t<Expression>;
  };

  template <typename ValueType, typename Expression>
  constexpr auto is_aggregate_v<sql_cast_t<ValueType, Expression>> = is_aggregate_v<Expression>;

  template <typename ValueType, typename Expression>
  [[nodiscard]] auto sql_cast(Expression expression)
  {
#warning: Need to check that ValueType is a valueType and Expression is an expression
    return sql_cast_t<ValueType, Expression>{expression};
  }

  template <typename Context, typename ValueType, typename Expression>
  [[nodiscard]] auto to_sql_string(Context& context, const sql_cast_t<ValueType, Expression>& t)
  {
    return " CAST(" + to_sql_string(context, t._expression) + " AS " + value_type_to_sql_string(context, type_t<ValueType>{}) + ")";
  }

}  // namespace sqlpp
