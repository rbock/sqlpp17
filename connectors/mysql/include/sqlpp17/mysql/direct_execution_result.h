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
#include <string>
#include <string_view>

#include <sqlpp17/result_row.h>

#include <sqlpp17/mysql/mysql.h>

namespace sqlpp ::mysql::detail
{
  struct result_cleanup_t
  {
    auto operator()(MYSQL_RES* result) const -> void
    {
      if (result)
        mysql_free_result(result);
    }
  };
  using unique_result_ptr = std::unique_ptr<MYSQL_RES, result_cleanup_t>;

  inline auto assert_field(char* data) -> void
  {
    if (data == nullptr)
      throw std::logic_error("Trying to obtain NULL for non-nullable value");
  }

}  // namespace sqlpp::mysql::detail

namespace sqlpp ::mysql
{
  inline auto read_field(char* data, unsigned long length, bool& value) -> void
  {
    detail::assert_field(data);
    switch (data[0])
    {
      case 't': [[fall_through]];
      case '1': value = true;
      default: value = false;
    }
  }

  inline auto read_field(char* data, unsigned long length, std::int32_t& value) -> void
  {
    detail::assert_field(data);
    value = std::strtol(data, nullptr, 10);
  }

  inline auto read_field(char* data, unsigned long length, std::int64_t& value) -> void
  {
    detail::assert_field(data);
    value = std::strtoll(data, nullptr, 10);
  }

  inline auto read_field(char* data, unsigned long length, float& value) -> void
  {
    detail::assert_field(data);
    value = std::strtof(data, nullptr);
  }

  inline auto read_field(char* data, unsigned long length, double& value) -> void
  {
    detail::assert_field(data);
    value = std::strtod(data, nullptr);
  }

  inline auto read_field(char* data, unsigned long length, std::string_view& value) -> void
  {
    detail::assert_field(data);
    value = std::string_view(data, length);
  }

  template <typename T>
  auto read_field(char* data, unsigned long length, std::optional<T>& value) -> void
  {
    if (not data)
    {
      value.reset();
    }
    else
    {
      value = T{};
      read_field(data, length, *value);
    }
  }

  template <typename... ColumnSpecs>
  auto read_fields(MYSQL_ROW data, unsigned long* lengths, result_row_t<ColumnSpecs...>& row) -> void
  {
    if (!data)
      throw std::logic_error("Trying to obtain value from non-existing row");

    std::size_t index = 0;
    (..., (read_field(data[index], lengths[index], static_cast<result_column_base<ColumnSpecs>&>(row)()), ++index));
  }

  template <typename ResultRow>
  class direct_execution_result_t
  {
    static_assert(wrong<ResultRow>, "ResultRow must be a result_row_t<...>");
  };

  template <typename... ColumnSpecs>
  class direct_execution_result_t<result_row_t<ColumnSpecs...>>
  {
    detail::unique_result_ptr _handle;
    MYSQL_ROW _data = nullptr;
    unsigned long* _lengths = nullptr;
    result_row_t<ColumnSpecs...> _row;

  public:
    using row_type = decltype(_row);

    direct_execution_result_t() = default;
    direct_execution_result_t(detail::unique_result_ptr handle)
        : _handle(std::move(handle))
    {
    }
    direct_execution_result_t(const direct_execution_result_t&) = delete;
    direct_execution_result_t(direct_execution_result_t&& rhs) = default;
    direct_execution_result_t& operator=(const direct_execution_result_t&) = delete;
    direct_execution_result_t& operator=(direct_execution_result_t&&) = default;
    ~direct_execution_result_t() = default;

    [[nodiscard]] operator bool() const
    {
      return !!_handle;
    }

    auto get_next_row() -> void
    {
      _data = mysql_fetch_row(_handle.get());
      _lengths = mysql_fetch_lengths(_handle.get());

      if (_data != nullptr)
      {
        read_fields(_data, _lengths, _row);
      }
      else
      {
        reset();
      }
    }

    [[nodiscard]] auto& row() const
    {
      return _row;
    }

    auto* get() const
    {
      return _handle.get();
    }

    auto* get_data() const
    {
      return _data;
    }

    auto* get_lengths() const
    {
      return _lengths;
    }

    auto reset()
    {
      *this = direct_execution_result_t{};
    }
  };
}  // namespace sqlpp::mysql

