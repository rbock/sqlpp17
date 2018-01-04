#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
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

#include <string_view>
#include <utility>

namespace sqlpp
{
  template <char...>
  struct char_sequence
  {
    template <char... Cs>
    constexpr bool operator==(const char_sequence<Cs...>& rhs) const
    {
      return std::is_same_v<char_sequence, char_sequence<Cs...>>;
    }
  };

  namespace detail
  {
    template <const std::string_view& Text, typename IndexSequence>
    struct make_char_sequence;

    template <const std::string_view& Text, std::size_t... Is>
    struct make_char_sequence<Text, std::index_sequence<Is...>>
    {
      using type = char_sequence<Text[Is]...>;
    };
  }  // namespace detail

  template <const std::string_view& Text>
  using make_char_sequence_t = typename detail::make_char_sequence<Text, std::make_index_sequence<Text.size()>>::type;

}  // namespace sqlpp
