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
#include <sqlpp17/type_traits.h>
#include <sqlpp17/member.h>

namespace sqlpp
{
  template <typename T>
  using cpp_type = int;
#warning : Need to have "real" cpp_type

  namespace detail
  {
    template <typename Selected, bool CanBeNull, bool NullIsTrivialValue>
    struct make_result_field_base;

    template <typename Selected, bool NullIsTrivialValue>
    struct make_result_field_base<Selected, false, NullIsTrivialValue>
    {
      using type = typename Selected::_alias_t::template _member_t<cpp_type<Selected>>;
    };

    template <typename Selected>
    struct make_result_field_base<Selected, true, false>
    {
      using type = typename Selected::_alias_t::template _member_t<can_be_null<cpp_type<Selected>>>;
    };

    template <typename Selected>
    struct make_result_field_base<Selected, true, true>
    {
      using type = typename Selected::_alias_t::template _member_t<std::optional<cpp_type<Selected>>>;
    };
  }

  template <typename Selected>
  using result_field_base = typename detail::
      make_result_field_base<Selected, can_be_null_v<Selected>, null_is_trivial_value_v<Selected>>::type;
}
