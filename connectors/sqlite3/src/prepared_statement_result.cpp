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

#include <sqlpp17/sqlite3/prepared_statement_result.h>

namespace sqlpp::sqlite3::detail
{
  auto get_next_result_row(prepared_statement_result_t& result) -> bool
  {
    auto rc = sqlite3_step(result.get());

    switch (rc)
    {
      case SQLITE_ROW:
        return true;
      case SQLITE_DONE:
        return false;
      default:
        throw sqlpp::exception("Sqlite3 error: Unexpected return value for sqlite3_step(): " +
                               std::string(sqlite3_errstr(rc)));
    }
  }
}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  auto post_bind_field(prepared_statement_result_t& result, bool& value, int index) -> void
  {
    value = sqlite3_column_int(result.get(), index);
  }

  auto post_bind_field(prepared_statement_result_t& result, std::int32_t& value, int index) -> void
  {
    value = sqlite3_column_int(result.get(), index);
  }

  auto post_bind_field(prepared_statement_result_t& result, std::int64_t& value, int index) -> void
  {
    value = sqlite3_column_int64(result.get(), index);
  }

  auto post_bind_field(prepared_statement_result_t& result, float& value, int index) -> void
  {
    // There is no column_float
    value = sqlite3_column_double(result.get(), index);
  }

  auto post_bind_field(prepared_statement_result_t& result, double& value, int index) -> void
  {
    value = sqlite3_column_double(result.get(), index);
  }

  auto post_bind_field(prepared_statement_result_t& result, std::string_view& value, int index) -> void
  {
    value = std::string_view{reinterpret_cast<const char*>(sqlite3_column_text(result.get(), index)),
                             static_cast<std::size_t>(sqlite3_column_bytes(result.get(), index))};
  }

}  // namespace sqlpp::sqlite3

