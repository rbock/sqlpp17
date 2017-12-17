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

#include <sqlpp17/exception.h>

#include <sqlpp17/mysql/bind_result.h>

namespace sqlpp::mysql::detail
{
  auto bind_impl(bind_result_t& result) -> void
  {
    if (result.debug())
      result.debug()("Binding results");

    if (mysql_stmt_bind_result(result.get(), result.get_bind_data().data()))
    {
      throw sqlpp::exception(std::string("MySQL: mysql_stmt_bind_result: ") + mysql_stmt_error(result.get()));
    }
  }

  auto get_next_result_row(bind_result_t& result) -> bool
  {
    if (result.debug())
      result.debug()("Reading bound row");

    auto flag = mysql_stmt_fetch(result.get());

    switch (flag)
    {
      case 0:
      case MYSQL_DATA_TRUNCATED:
      {
        bool need_to_rebind = false;
        std::size_t index = 0;
        for (auto& r : result.get_bind_meta_data())
        {
          if (r.use_buffer and r.bound_len > r.bound_buffer.size())
          {
            if (result.debug())
              result.debug()("Reallocating buffer at index " + std::to_string(index));
            need_to_rebind = true;
            r.bound_buffer.resize(r.bound_len);
            MYSQL_BIND& param = result.get_bind_data()[index];
            param.buffer = r.bound_buffer.data();
            param.buffer_length = r.bound_buffer.size();

            auto err = mysql_stmt_fetch_column(result.get(), &result.get_bind_data()[index], index, 0);
            if (err)
              throw sqlpp::exception(std::string("MySQL: Fetch column after reallocate failed: ") + "error-code: " +
                                     std::to_string(err) + ", stmt-error: " + mysql_stmt_error(result.get()) +
                                     ", stmt-errno: " + std::to_string(mysql_stmt_errno(result.get())));
          }
          ++index;
        }
        if (need_to_rebind)
          detail::bind_impl(result);
      }
        return true;
      case 1:
        throw sqlpp::exception(std::string("MySQL: Could not fetch next result: ") + mysql_stmt_error(result.get()));
      case MYSQL_NO_DATA:
        return false;
      default:
        throw sqlpp::exception("MySQL: Unexpected return value for mysql_stmt_fetch()");
    }
  }
}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  auto pre_bind_field(bind_result_t& result, std::int64_t& value, std::size_t index) -> void
  {
    if (result.debug())
      result.debug()("Binding integral result at index " + std::to_string(index));

    detail::bind_meta_data_t& meta_data = result.get_bind_meta_data()[index];

    MYSQL_BIND& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_LONGLONG;
    param.buffer = &value;
    param.buffer_length = sizeof(value);
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }
}  // namespace sqlpp::mysql

