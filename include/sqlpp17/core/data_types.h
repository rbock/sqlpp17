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

#include <sqlpp17/core/type_traits.h>

namespace sqlpp
{
  template <uint8_t Size>
  struct fixchar
  {
  };

  template <uint8_t Size>
  constexpr auto is_text_v<fixchar<Size>> = true;

  template <uint8_t Size>
  struct cpp_type<fixchar<Size>>
  {
    using type = std::string_view;
  };

  template <uint8_t Size>
  struct varchar
  {
  };

  template <uint8_t Size>
  constexpr auto is_text_v<varchar<Size>> = true;

  template <uint8_t Size>
  struct cpp_type<varchar<Size>>
  {
    using type = std::string_view;
  };

  struct text
  {
  };

  template <>
  constexpr auto is_text_v<text> = true;

  template <>
  struct cpp_type<text>
  {
    using type = std::string_view;
  };

}  // namespace sqlpp
