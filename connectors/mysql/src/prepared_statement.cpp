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

#include <sqlpp17/mysql/prepared_statement.h>

namespace sqlpp::mysql
{
  auto bind_parameter(prepared_statement_t& statement, const std::nullopt_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding parameter NULL at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = true;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_NULL;
    parameter.buffer = nullptr;
    parameter.buffer_length = 0;
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, bool& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding bool parameter '" + std::to_string(value) + "' at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = false;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_TINY;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, std::int32_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding int64_t parameter " + std::to_string(value) + " at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = false;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_LONG;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, std::int64_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding int64_t parameter " + std::to_string(value) + " at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = false;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_LONGLONG;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, std::string& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding string parameter '" + value + "' at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = false;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_STRING;
    parameter.buffer = value.data();
    parameter.buffer_length = value.size();
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, std::string_view& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding string_view parameter '" + std::string(value) +
                        "' at index: " + std::to_string(index));

    auto& meta_data = statement.get_bind_meta_data()[index];
    meta_data.bound_is_null = false;

    auto& parameter = statement.get_bind_data()[index];
    parameter.is_null = &meta_data.bound_is_null;
    parameter.buffer_type = MYSQL_TYPE_STRING;
    parameter.buffer = const_cast<char*>(value.data());  // Sigh...
    parameter.buffer_length = value.size();
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

}  // namespace sqlpp::mysql

