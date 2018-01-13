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

#include <string>

#include <sqlpp17/postgresql/prepared_statement.h>

namespace sqlpp::postgresql
{
  auto bind_parameter(prepared_statement_t& statement, const std::nullopt_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding parameter NULL at index: " + std::to_string(index));

    statement.get_parameter_pointers()[index] = nullptr;
  }

  auto bind_parameter(prepared_statement_t& statement, const bool& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding bool parameter '" + (value ? std::string("TRUE") : std::string("FALSE")) +
                        "' at index: " + std::to_string(index));

    statement.get_parameter_data()[index] = value ? "TRUE" : "FALSE";
    statement.get_parameter_pointers()[index] = statement.get_parameter_data()[index].data();
  }

  auto bind_parameter(prepared_statement_t& statement, const std::int32_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding int32_t parameter '" + std::to_string(value) + "' at index: " + std::to_string(index));

    statement.get_parameter_data()[index] = std::to_string(value);
    statement.get_parameter_pointers()[index] = statement.get_parameter_data()[index].data();
  }

  auto bind_parameter(prepared_statement_t& statement, const std::int64_t& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding int64_t parameter '" + std::to_string(value) + "' at index: " + std::to_string(index));

    statement.get_parameter_data()[index] = std::to_string(value);
    statement.get_parameter_pointers()[index] = statement.get_parameter_data()[index].data();
  }

  auto bind_parameter(prepared_statement_t& statement, const std::string& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding string parameter '" + value + "' at index: " + std::to_string(index));

    statement.get_parameter_data()[index] = value;
    statement.get_parameter_pointers()[index] = statement.get_parameter_data()[index].data();
  }

  auto bind_parameter(prepared_statement_t& statement, const std::string_view& value, int index) -> void
  {
    if (statement.debug())
      statement.debug()("binding string_view parameter '" + std::string(value) +
                        "' at index: " + std::to_string(index));

    statement.get_parameter_data()[index] = value;
    statement.get_parameter_pointers()[index] = statement.get_parameter_data()[index].data();
  }

}  // namespace sqlpp::postgresql
