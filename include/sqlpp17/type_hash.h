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

#include <string_view>

namespace sqlpp
{
  // From https://en.wikipedia.org/wiki/Universal_hashing#Hashing_strings
  constexpr auto djb2_hash(const std::string_view& input) -> std::uint32_t
  {
    std::uint32_t _hash = 5381;
    for (auto c : input)
    {
      _hash = _hash * 33 + c;
    }
    return _hash;
  }

  // From https://stackoverflow.com/a/35943472/2173029
  template <typename T>
  constexpr auto type_hash()
  {
#ifdef _MSC_VER
    constexpr auto _name = std::string_view{__FUNCSIG__};
#else
    constexpr auto _name = std::string_view{__PRETTY_FUNCTION__};
#endif
    return djb2_hash(_name);
  }

  template <typename T>
  constexpr auto type_hash([[maybe_unused]] T)
  {
    return type_hash<T>();
  }
}  // namespace sqlpp

