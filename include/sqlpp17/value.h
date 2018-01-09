#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <string>
#include <string_view>

#include <sqlpp17/expr.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename T>
  struct value_t
  {
    T t;
  };

  template <typename T>
  struct nodes_of<value_t<T>>
  {
    using type = type_vector<T>;
  };

  template <typename T>
  struct value_type_of<value_t<T>>
  {
    using type = T;
  };

  template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  [[nodiscard]] constexpr auto value(T t)
  {
    return expr(value_t<T>{t});
  }

  [[nodiscard]] auto value(std::string t)
  {
    return expr(value_t<std::string>{t});
  }

  [[nodiscard]] constexpr auto value(std::string_view t)
  {
    return expr(value_t<std::string_view>{t});
  }

  [[nodiscard]] constexpr auto value(const char* t)
  {
    return expr(value_t<const char*>{t});
  }

  template <typename Context, typename T>
  [[nodiscard]] auto to_sql_string(Context& context, const value_t<T>& t)
  {
    return to_sql_string(context, t.t);
  }
}  // namespace sqlpp
