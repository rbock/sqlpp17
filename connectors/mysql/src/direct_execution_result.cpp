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

#include <sqlpp17/mysql/direct_execution_result.h>

namespace sqlpp::mysql::detail
{
  auto get_next_result_row(direct_execution_result_t& result) -> bool
  {
    if (result.debug())
      result.debug()("Reading char row");

    result._data = mysql_fetch_row(result.get());
    result._lengths = mysql_fetch_lengths(result.get());

    return !!result._data;
  }
}  // namespace sqlpp::mysql::detail

namespace
{
  auto assert_row(sqlpp::mysql::direct_execution_result_t& result) -> void
  {
    if (!result.get_data())
      throw std::logic_error("Trying to obtain value from non-existing row");
  }

  auto assert_field(sqlpp::mysql::direct_execution_result_t& result, int index) -> void
  {
    assert_row(result);
    if (!result.get_data()[index])
      throw std::logic_error("Trying to obtain NULL for non-nullable value");
  }
}  // namespace

namespace sqlpp::mysql
{
  auto bind_field(direct_execution_result_t& result, std::int64_t& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding int64_t result at index " + std::to_string(index));

    assert_field(result, index);
    value = std::strtoll(result.get_data()[index], nullptr, 10);
  }

  auto bind_field(direct_execution_result_t& result, std::int32_t& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding int32_t result at index " + std::to_string(index));

    assert_field(result, index);
    value = std::strtol(result.get_data()[index], nullptr, 10);
  }

  auto bind_field(direct_execution_result_t& result, float& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding float result at index " + std::to_string(index));

    assert_field(result, index);
    value = std::strtof(result.get_data()[index], nullptr);
  }

  auto bind_field(direct_execution_result_t& result, double& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding double result at index " + std::to_string(index));

    assert_field(result, index);
    value = std::strtod(result.get_data()[index], nullptr);
  }

  auto bind_field(direct_execution_result_t& result, std::string_view& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding string_view result at index " + std::to_string(index));

    assert_field(result, index);
    value = std::string_view(result.get_data()[index], result.get_lengths()[index]);
  }

}  // namespace sqlpp::mysql
