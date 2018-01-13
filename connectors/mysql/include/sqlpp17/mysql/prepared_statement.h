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
#include <vector>

#include <mysql.h>

namespace sqlpp::mysql::detail
{
  struct bind_meta_data_t
  {
    unsigned long bound_len;
    my_bool bound_is_null;
    my_bool bound_error;
    bool use_buffer = false;
    std::vector<char> bound_buffer;
  };

  struct prepared_statement_cleanup_t
  {
  public:
    auto operator()(MYSQL_STMT* handle) -> void
    {
      if (handle)
        mysql_stmt_close(handle);
    }
  };
  using unique_prepared_statement_ptr = std::unique_ptr<MYSQL_STMT, detail::prepared_statement_cleanup_t>;

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  class prepared_statement_t
  {
    detail::unique_prepared_statement_ptr _handle;
    std::size_t _number_of_parameters;
    std::size_t _number_of_columns;
    std::vector<detail::bind_meta_data_t> _bind_meta_data;
    std::vector<MYSQL_BIND> _bind_data;
    std::function<void(std::string_view)> _debug;

  public:
    prepared_statement_t() = default;
    prepared_statement_t(detail::unique_prepared_statement_ptr handle,
                         std::size_t number_of_parameters,
                         std::size_t number_of_columns,
                         std::function<void(std::string_view)> debug)
        : _handle(std::move(handle)),
          _number_of_parameters(number_of_parameters),
          _number_of_columns(number_of_columns),
          _bind_meta_data(number_of_parameters),
          _bind_data(number_of_parameters),
          _debug(debug)
    {
    }
    prepared_statement_t(const prepared_statement_t&) = delete;
    prepared_statement_t(prepared_statement_t&& rhs) = default;
    prepared_statement_t& operator=(const prepared_statement_t&) = delete;
    prepared_statement_t& operator=(prepared_statement_t&&) = default;
    ~prepared_statement_t() = default;

    auto* get() const
    {
      return _handle.get();
    }

    auto get_number_of_parameters() const
    {
      return _number_of_parameters;
    }

    auto get_number_of_columns() const
    {
      return _number_of_columns;
    }

    auto& get_bind_meta_data()
    {
      return _bind_meta_data;
    }

    auto& get_bind_data()
    {
      return _bind_data;
    }

    auto debug() const
    {
      return _debug;
    }
  };

  auto bind_parameter(prepared_statement_t& statement, const std::nullopt_t& value, int index) -> void;

  // Taking parameters by non-const reference to prevent temporaries being created which
  // would lead to dangling pointers in the implementation
  auto bind_parameter(prepared_statement_t& statement, bool& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, std::int32_t& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, std::int64_t& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, std::string& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, std::string_view& value, int index) -> void;

  template <typename T>
  auto bind_parameter(prepared_statement_t& statement, std::optional<T>& value, int index) -> void
  {
    value ? bind_parameter(statement, *value, index) : bind_parameter(statement, std::nullopt, index);
  }

}  // namespace sqlpp::mysql

