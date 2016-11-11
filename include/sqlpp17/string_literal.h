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

#include <cstddef>

namespace sqlpp
{
  class string_literal
  {
    const char* const _content;
    std::size_t _size;

  public:
    template <unsigned N>
    constexpr string_literal(const char (&content)[N]) : _content(content), _size(N)
    {
    }

    constexpr auto get() const
    {
      return _content;
    }

    constexpr auto at(std::size_t i) const
    {
      return _content[i];
    }

    constexpr auto size() const
    {
      return _size;
    }

    constexpr auto operator==(const string_literal& rhs) const
    {
      if (size() != rhs.size())
        return false;
      for (auto i = std::size_t{}; i < size(); ++i)
      {
        if (get()[i] != rhs.get()[i])
          return false;
      }
      return true;
    }

    constexpr auto operator!=(const string_literal& rhs) const
    {
      return !(operator==(rhs));
    }
  };
}
