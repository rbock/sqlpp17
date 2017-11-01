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

#include <sqlpp17/result_column_base.h>

namespace sqlpp
{
  template <typename... ColumnSpecs>
  struct result_row_t : result_column_base<ColumnSpecs>...
  {
  };

  template <typename... LeftColumnSpecs, typename... RightColumnSpecs>
  struct result_rows_are_compatible<result_row_t<LeftColumnSpecs...>, result_row_t<RightColumnSpecs...>>
  {
    static constexpr auto get_value()
    {
      if constexpr (sizeof...(LeftColumnSpecs) != sizeof...(RightColumnSpecs))
      {
        return false;
      }
      else
      {
        return (true && ... &&
                (char_sequence_of_v<LeftColumnSpecs> == char_sequence_of_v<RightColumnSpecs> and
                 std::is_same_v<value_type_of_t<LeftColumnSpecs>, value_type_of_t<RightColumnSpecs>>));
      }
    }

    static constexpr auto value = get_value();
  };
}  // namespace sqlpp
