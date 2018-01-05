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

#include <sqlpp17/operator.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename ValueType, typename Column>
  struct result_cast_t
  {
    Column _column;
  };

  template <typename ValueType, typename Column>
  struct nodes_of<result_cast_t<ValueType, Column>>
  {
    using type = type_vector<Column>;
  };

  template <typename ValueType, typename Column>
  struct value_type_of<result_cast_t<ValueType, Column>>
  {
    using type = ValueType;
  };

  template <typename ValueType, typename Column>
  [[nodiscard]] auto result_cast(Column column)
  {
    return result_cast_t<ValueType, Column>{column};
  }

  template <typename Context, typename ValueType, typename Column>
  [[nodiscard]] auto to_sql_string(Context& context, const result_cast_t<ValueType, Column>& t)
  {
    return to_sql_string(context, t._column);
  }

}  // namespace sqlpp
