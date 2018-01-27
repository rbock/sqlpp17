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
    std::function<void(std::string_view)> _debug;
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
          _bind_data(statement.get_number_of_columns()),
          _debug(statement.debug())
    {
    }
    prepared_statement_result_t(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t(prepared_statement_result_t&& rhs) = default;
    prepared_statement_result_t& operator=(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t& operator=(prepared_statement_result_t&&) = default;
    ~prepared_statement_result_t(){}

        [[nodiscard]]
        operator bool() const
    {
      return !!_handle;
    }

    [[nodiscard]] auto* get() const
    {
      return _handle;
    }

    [[nodiscard]] auto debug() const
    {
      return _debug;
    }

#warning : This should be private
    [[nodiscard]] auto& get_bind_meta_data()
    {
      return _bind_meta_data;
    }

#warning : This should be private
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
    // prepare bind
    if (&row != result._result_row_address)
    {
      row.pre_bind(result);       // binds row data to statement data
      detail::bind_impl(result);  // bind statement data
      result._result_row_address = &row;
    }

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

  auto pre_bind_field(prepared_statement_result_t& result, std::int32_t& value, std::size_t index) -> void;
  auto pre_bind_field(prepared_statement_result_t& result, std::int64_t& value, std::size_t index) -> void;
  auto pre_bind_field(prepared_statement_result_t& result, float& value, std::size_t index) -> void;
  auto pre_bind_field(prepared_statement_result_t& result, double& value, std::size_t index) -> void;

  inline auto post_bind_field(prepared_statement_result_t& result, std::int64_t& value, std::size_t index) -> void
  {
  }

  inline auto post_bind_field(prepared_statement_result_t& result, std::int32_t& value, std::size_t index) -> void
  {
  }

  inline auto post_bind_field(prepared_statement_result_t& result, float& value, std::size_t index) -> void
  {
  }

  inline auto post_bind_field(prepared_statement_result_t& result, double& value, std::size_t index) -> void
  {
  }

  auto pre_bind_field(prepared_statement_result_t& result, std::optional<std::string_view>& value, std::size_t index)
      -> void;
  auto post_bind_field(prepared_statement_result_t& result, std::optional<std::string_view>& value, std::size_t index)
      -> void;

}  // namespace sqlpp::mysql
