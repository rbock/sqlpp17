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

#include <mysql.h>

namespace sqlpp ::mysql
{
  class direct_execution_result_t;
}

namespace sqlpp ::mysql::detail
{
  auto get_next_result_row(direct_execution_result_t& result) -> bool;

  struct result_cleanup_t
  {
    auto operator()(MYSQL_RES* result) const -> void
    {
      if (result)
        mysql_free_result(result);
    }
  };
  using unique_result_ptr = std::unique_ptr<MYSQL_RES, result_cleanup_t>;

}  // namespace sqlpp::mysql::detail

namespace sqlpp ::mysql
{
  class direct_execution_result_t
  {
    detail::unique_result_ptr _handle;
    std::function<void(std::string_view)> _debug;
    MYSQL_ROW _data = nullptr;
    unsigned long* _lengths = nullptr;

    friend auto sqlpp ::mysql::detail::get_next_result_row(direct_execution_result_t& result) -> bool;

  public:
    direct_execution_result_t() = default;
    direct_execution_result_t(detail::unique_result_ptr handle, std::function<void(std::string_view)> debug)
        : _handle(std::move(handle)), _debug(debug)
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

    auto debug() const
    {
      return _debug;
    }

    auto reset()
    {
      *this = direct_execution_result_t{};
    }
  };

  template <typename Row>
  auto get_next_result_row(direct_execution_result_t& result, Row& row) -> void
  {
    if (detail::get_next_result_row(result))
    {
      row.bind(result);
    }
    else
    {
      result.reset();
    }
  }

  auto bind_field(direct_execution_result_t& result, std::int32_t& value, int index) -> void;
  auto bind_field(direct_execution_result_t& result, std::int64_t& value, int index) -> void;
  auto bind_field(direct_execution_result_t& result, float& value, int index) -> void;
  auto bind_field(direct_execution_result_t& result, double& value, int index) -> void;
  auto bind_field(direct_execution_result_t& result, std::string_view& value, int index) -> void;

  template <typename T>
  auto bind_field(direct_execution_result_t& result, std::optional<T>& value, int index) -> void
  {
    if (result.debug())
      result.debug()("Binding optional result at index " + std::to_string(index));

    if (!result.get_data()[index])
    {
      value.reset();
    }
    else
    {
      value = T{};
      bind_field(result, *value, index);
    }
  }

}  // namespace sqlpp::mysql

