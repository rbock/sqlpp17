#pragma once

/*
Copyright (c) 2017 - 2019, Roland Bock
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

#include <sqlpp17/core/value_type_to_sql_string.h>

namespace sqlpp::sqlite3
{
  struct context_t;
}

namespace sqlpp
{
  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<bool>)
  {
    return " INTEGER";
  }

  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<int32_t>)
  {
    return " INTEGER";
  }

  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<int64_t>)
  {
    return " INTEGER";
  }

  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<float>)
  {
    return " FLOAT";
  }

  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<double>)
  {
    return " DOUBLE";
  }

  template <uint8_t Size>
  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<::sqlpp::fixchar<Size>>)
  {
    return " TEXT";
  }

  template <uint8_t Size>
  [[nodiscard]] inline auto value_type_to_sql_string(::sqlpp::sqlite3::context_t&, type_t<::sqlpp::varchar<Size>>)
  {
    return " TEXT";
  }

}  // namespace sqlpp
