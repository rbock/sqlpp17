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

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#include <mysql.h>

#include <sqlpp17/exception.h>

#include <sqlpp17/mysql/prepared_statement.h>

namespace sqlpp::mysql
{
  class prepared_statement_result_t;
  class prepared_statement_t;
}  // namespace sqlpp::mysql

namespace sqlpp::mysql::detail
{
  auto bind_impl(prepared_statement_result_t& result) -> void;
  auto get_next_result_row(prepared_statement_result_t& result) -> bool;
}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  class prepared_statement_result_t
  {
    MYSQL_STMT* _handle = nullptr;
    std::vector<detail::bind_meta_data_t> _bind_meta_data;
    std::vector<MYSQL_BIND> _bind_data;
    void* _result_row_address = nullptr;

    friend auto detail::bind_impl(prepared_statement_result_t& result) -> void;
    friend auto detail::get_next_result_row(prepared_statement_result_t& result) -> bool;
    template <typename Row>
    friend auto get_next_result_row(prepared_statement_result_t& result, Row& row) -> void;

  public:
    prepared_statement_result_t() = default;
    prepared_statement_result_t(::sqlpp::mysql::prepared_statement_t& statement)
        : _handle(statement.get()),
          _bind_meta_data(statement.get_number_of_columns()),
          _bind_data(statement.get_number_of_columns())
    {
    }

    prepared_statement_result_t(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t(prepared_statement_result_t&& rhs)
    {
      std::swap(_handle, rhs._handle);
      std::swap(_bind_meta_data, rhs._bind_meta_data);
      std::swap(_bind_data, rhs._bind_data);
    }
    prepared_statement_result_t& operator=(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t& operator=(prepared_statement_result_t&& rhs)
    {
      std::swap(_handle, rhs._handle);
      std::swap(_bind_meta_data, rhs._bind_meta_data);
      std::swap(_bind_data, rhs._bind_data);
      return *this;
    }

    ~prepared_statement_result_t()
    {
      if (_handle)
      {
        mysql_stmt_free_result(_handle);
      }
    }

    [[nodiscard]] operator bool() const
    {
      return !!_handle;
    }

    [[nodiscard]] auto* get() const
    {
      return _handle;
    }

    [[nodiscard]] auto& get_bind_meta_data()
    {
      return _bind_meta_data;
    }

    [[nodiscard]] auto& get_bind_data()
    {
      return _bind_data;
    }

    auto reset() -> void
    {
      operator=(prepared_statement_result_t{});
    }
  };

  template <typename Row>
  auto get_next_result_row(prepared_statement_result_t& result, Row& row) -> void
  {
    // Prepare reading results
    // This binds memory of all non-optional values
    if (&row != result._result_row_address)
    {
      row.pre_bind(result);       // binds row data to statement data
      detail::bind_impl(result);  // bind statement data
      result._result_row_address = &row;
    }

    // This binds optional values.
    // It has the side effect of turning all NULL values into T{}
    // before reading the next row (otherwise there would be nothing to bind).
    row.bind(result);

    if (detail::get_next_result_row(result))
    {
      // post-process bound fields, where necessary
      row.post_bind(result);
    }
    else
    {
      result.reset();
    }
  }

  inline auto pre_bind_field(prepared_statement_result_t& result, std::int32_t& value, std::size_t index) -> void
  {
    auto& meta_data = result.get_bind_meta_data()[index];

    auto& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_LONG;
    param.buffer = &value;
    param.buffer_length = sizeof(value);
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }

  inline auto pre_bind_field(prepared_statement_result_t& result, std::int64_t& value, std::size_t index) -> void
  {
    auto& meta_data = result.get_bind_meta_data()[index];

    auto& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_LONGLONG;
    param.buffer = &value;
    param.buffer_length = sizeof(value);
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }

  inline auto pre_bind_field(prepared_statement_result_t& result, float& value, std::size_t index) -> void
  {
    auto& meta_data = result.get_bind_meta_data()[index];

    auto& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_FLOAT;
    param.buffer = &value;
    param.buffer_length = sizeof(value);
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }

  inline auto pre_bind_field(prepared_statement_result_t& result, double& value, std::size_t index) -> void
  {
    auto& meta_data = result.get_bind_meta_data()[index];

    auto& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_DOUBLE;
    param.buffer = &value;
    param.buffer_length = sizeof(value);
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }

  inline auto pre_bind_field(prepared_statement_result_t& result,
                             [[maybe_unused]] std::string_view& value,
                             std::size_t index) -> void
  {
    auto& meta_data = result.get_bind_meta_data()[index];
    meta_data.use_buffer = true;

    auto& param = result.get_bind_data()[index];
    param.buffer_type = MYSQL_TYPE_STRING;
    param.buffer = meta_data.bound_buffer.data();
    param.buffer_length = meta_data.bound_buffer.size();
    param.length = &meta_data.bound_len;
    param.is_null = &meta_data.bound_is_null;
    param.is_unsigned = false;
    param.error = &meta_data.bound_error;
  }

  template <typename T>
  inline auto pre_bind_field(prepared_statement_result_t& result, std::optional<T>& value, std::size_t index) -> void
  {
    if (not value)
      value = {};
    pre_bind_field(result, *value, index);
  }

  inline auto bind_field(prepared_statement_result_t& result, ...) -> void
  {
  }

  template <typename T>
  inline auto bind_field(prepared_statement_result_t& result, std::optional<T>& value, std::size_t index) -> void
  {
    if (not value)
    {
      value = {};
      pre_bind_field(result, value, index);
    }
  }

  inline auto post_bind_field(prepared_statement_result_t& result, ...) -> void
  {
  }

  inline auto post_bind_field(prepared_statement_result_t& result, std::string_view& value, std::size_t index) -> void
  {
    const auto& meta_data = result.get_bind_meta_data()[index];

    value = std::string_view{meta_data.bound_buffer.data(), meta_data.bound_len};
  }

  template<typename T>
  inline auto post_bind_field(prepared_statement_result_t& result,
                              std::optional<T>& value,
                              std::size_t index) -> void
  {
    const auto& meta_data = result.get_bind_meta_data()[index];

    if (meta_data.bound_is_null)
    {
      value.reset();
    }
    else
    {
      post_bind_field(result, *value, index);
    }
  }

}  // namespace sqlpp::mysql

namespace sqlpp::mysql::detail
{
  inline auto bind_impl(prepared_statement_result_t& result) -> void
  {
    if (mysql_stmt_bind_result(result.get(), result.get_bind_data().data()))
    {
      throw sqlpp::exception(std::string("MySQL: mysql_stmt_prepared_statement_result: ") +
                             mysql_stmt_error(result.get()));
    }
  }

  inline auto get_next_result_row(prepared_statement_result_t& result) -> bool
  {
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

