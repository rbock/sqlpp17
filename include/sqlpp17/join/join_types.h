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

#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  struct cross_join_t
  {
    template <typename Lhs, typename Rhs>
    static constexpr auto _can_be_null_columns_of = (can_be_null_columns_of_v<Lhs> | can_be_null_columns_of_v<Rhs>);

    static constexpr const char* _name = " CROSS";
  };

  struct inner_join_t
  {
    template <typename Lhs, typename Rhs>
    static constexpr auto _can_be_null_columns_of = (can_be_null_columns_of_v<Lhs> | can_be_null_columns_of_v<Rhs>);

    static constexpr const char* _name = " INNER";
  };

  struct outer_join_t
  {
    template <typename Lhs, typename Rhs>
    static constexpr auto _can_be_null_columns_of = (columns_of_v<Lhs> | columns_of_v<Rhs>);

    static constexpr const char* _name = " OUTER";
  };

  struct left_outer_join_t
  {
    template <typename Lhs, typename Rhs>
    static constexpr auto _can_be_null_columns_of = (can_be_null_columns_of_v<Lhs> | columns_of_v<Rhs>);

    static constexpr const char* _name = " LEFT OUTER";
  };

  struct right_outer_join_t
  {
    template <typename Lhs, typename Rhs>
    static constexpr auto _can_be_null_columns_of = (columns_of_v<Lhs> | can_be_null_columns_of_v<Rhs>);

    static constexpr const char* _name = " RIGHT OUTER";
  };
}  // namespace sqlpp
