/*
Copyright (c) 2018 - 2018, Roland Bock
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

#include <sqlpp17/sqlite3/prepared_statement.h>

namespace
{
  void check_bind_result(int result, const char* const type)
  {
    switch (result)
    {
      case SQLITE_OK:
        return;
      case SQLITE_RANGE:
        throw sqlpp::exception("Sqlite3 error: " + std::string(type) + " bind value out of range");
      case SQLITE_NOMEM:
        throw sqlpp::exception("Sqlite3 error: " + std::string(type) + " bind out of memory");
      default:
        throw sqlpp::exception("Sqlite3 error: " + std::string(type) +
                               " bind returned unexpected value: " + std::to_string(result));
    }
  }
}  // namespace

namespace sqlpp::sqlite3
{
  auto bind_parameter(prepared_statement_t& statement, [[maybe_unused]] const std::nullopt_t&, int index) -> void
  {
    const auto result = sqlite3_bind_null(statement.get(), index + 1);
    check_bind_result(result, "NULL");
  }

  auto bind_parameter(prepared_statement_t& statement, bool& value, int index) -> void
  {
    const auto result = sqlite3_bind_int(statement.get(), index + 1, value);
    check_bind_result(result, "bool");
  }

  auto bind_parameter(prepared_statement_t& statement, std::int32_t& value, int index) -> void
  {
    const auto result = sqlite3_bind_int(statement.get(), index + 1, value);
    check_bind_result(result, "int32_t");
  }

  auto bind_parameter(prepared_statement_t& statement, std::int64_t& value, int index) -> void
  {
    const auto result = sqlite3_bind_int64(statement.get(), index + 1, value);
    check_bind_result(result, "int64_t");
  }

  auto bind_parameter(prepared_statement_t& statement, float& value, int index) -> void
  {
    // There is no bind_float
    const auto result = sqlite3_bind_double(statement.get(), index + 1, value);
    check_bind_result(result, "float");
  }

  auto bind_parameter(prepared_statement_t& statement, double& value, int index) -> void
  {
    const auto result = sqlite3_bind_double(statement.get(), index + 1, value);
    check_bind_result(result, "double");
  }

  auto bind_parameter(prepared_statement_t& statement, std::string& value, int index) -> void
  {
    const auto result =
        sqlite3_bind_text(statement.get(), index + 1, value.data(), static_cast<int>(value.size()), SQLITE_STATIC);
    check_bind_result(result, "string");
  }

  auto bind_parameter(prepared_statement_t& statement, std::string_view& value, int index) -> void
  {
    const auto result =
        sqlite3_bind_text(statement.get(), index + 1, value.data(), static_cast<int>(value.size()), SQLITE_STATIC);
    check_bind_result(result, "string_view");
  }

}  // namespace sqlpp::sqlite3

