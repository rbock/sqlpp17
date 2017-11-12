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

#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

namespace sqlpp
{
  template <typename DbConnection, typename T>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const std::optional<T>& o)
  {
    return o ? to_sql_string(connection, o.value()) : "NULL";
  }

  template <typename DbConnection>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const char& c)
  {
    return std::string(1, c);
  }

  template <typename DbConnection>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const std::string_view& s)
  {
    auto ret = std::string{"'"};
    for (const auto c : s)
    {
      if (c == '\'')
        ret.push_back(c);  // Escaping
      ret.push_back(c);
    }
    ret.push_back('\'');
    return ret;
  }

  template <typename DbConnection, typename T>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const T& i)
      -> std::enable_if_t<std::is_integral_v<T>, std::string>
  {
    return std::to_string(i);
  }

  template <typename DbConnection, typename T>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const T& f)
      -> std::enable_if_t<std::is_floating_point_v<T>, std::string>
  {
    // TODO: Once gcc and clang support to_chars, try that
    auto oss = std::ostringstream{};
    oss << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << f;
    return oss.str();
  }
}  // namespace sqlpp
