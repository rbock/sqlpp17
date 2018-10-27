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
#include <functional>

namespace sqlpp
{
  struct connection
  {
  };

  enum class debug
  {
    none,
    allowed
  };

  struct no_pool
  {
  };

  template <typename Pool>
  struct pool_base
  {
    Pool* _connection_pool = nullptr;

    pool_base() = default;

    pool_base(Pool* connection_pool) : _connection_pool{connection_pool}
    {
    }
  };

  template <>
  struct pool_base<::sqlpp::no_pool>
  {
  };

  template <::sqlpp::debug Debug>
  struct debug_base
  {
    std::function<void(std::string_view)> _debug;
  };

  template <>
  struct debug_base<::sqlpp::debug::none>
  {
    debug_base(const std::function<void(std::string_view)>&)
    {
    }
  };

}  // namespace sqlpp
