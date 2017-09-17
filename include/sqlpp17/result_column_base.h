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

#include <optional>

#include <sqlpp17/member.h>
#include <sqlpp17/null_is_trivial.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  namespace detail
  {
    template <typename ColumnSpec, bool CanBeNull, bool NullIsTrivialValue>
    struct make_result_column_base;

    template <typename ColumnSpec, bool NullIsTrivialValue>
    struct make_result_column_base<ColumnSpec, false, NullIsTrivialValue>
    {
      using type = member_t<ColumnSpec, cpp_type_of_t<value_type_of_t<ColumnSpec>>>;
    };

    template <typename ColumnSpec>
    struct make_result_column_base<ColumnSpec, true, true>
    {
      using type = member_t<ColumnSpec, null_is_trivial<cpp_type_of_t<value_type_of_t<ColumnSpec>>>>;
    };

    template <typename ColumnSpec>
    struct make_result_column_base<ColumnSpec, true, false>
    {
      using type = member_t<ColumnSpec, std::optional<cpp_type_of_t<value_type_of_t<ColumnSpec>>>>;
    };
  }

  template <typename ColumnSpec>
  using result_column_base = typename detail::
      make_result_column_base<ColumnSpec, can_be_null_v<ColumnSpec>, null_is_trivial_value_v<ColumnSpec>>::type;
}
