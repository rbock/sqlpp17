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

#include <sqlpp17/exception.h>
#include <sqlpp17/postgresql/char_result.h>

namespace sqlpp::postgresql
{
  auto bind_field(char_result_t& result, std::int32_t& value, int index) -> void
  {
    value = std::strtol(PQgetvalue(result.get(), result.get_row_index(), index), nullptr, 10);
  }

  auto bind_field(char_result_t& result, std::int64_t& value, int index) -> void
  {
    value = std::strtoll(PQgetvalue(result.get(), result.get_row_index(), index), nullptr, 10);
  }

  auto bind_field(char_result_t& result, float& value, int index) -> void
  {
    value = std::strtof(PQgetvalue(result.get(), result.get_row_index(), index), nullptr);
  }

  auto bind_field(char_result_t& result, double& value, int index) -> void
  {
    value = std::strtod(PQgetvalue(result.get(), result.get_row_index(), index), nullptr);
  }

  auto bind_field(char_result_t& result, std::string_view& value, int index) -> void
  {
    value = std::string_view(PQgetvalue(result.get(), result.get_row_index(), index),
                             PQgetlength(result.get(), result.get_row_index(), index));
  }

  auto bind_field(char_result_t& result, std::optional<std::string_view>& value, int index) -> void
  {
    value = PQgetisnull(result.get(), result.get_row_index(), index)
                ? std::nullopt
                : std::optional<std::string_view>{
                      std::string_view(PQgetvalue(result.get(), result.get_row_index(), index),
                                       PQgetlength(result.get(), result.get_row_index(), index))};
  }
}  // namespace sqlpp::postgresql
