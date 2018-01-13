#pragma once

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

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <libpq-fe.h>

namespace sqlpp::postgresql
{
  /* PGprepare CAN be informed about the nature of parameters using OIDs from pg_type.h
     e.g. TEXTOID or INT4OID
     However, it seems easier to pass type information in the query via $1:bigint for
     example. See https://www.postgresql.org/docs/10/static/libpq-exec.html
     Caveat: We need to store all parameters as strings. And postgresql then has to
             parse those strings.
  */
  class prepared_statement_t
  {
    PGconn* _connection;
    std::string _name;
    std::vector<std::string> _parameter_data;
    std::vector<char*> _parameter_pointers;
    std::function<void(std::string_view)> _debug;

  public:
    prepared_statement_t() = default;
    prepared_statement_t(PGconn* connection,
                         std::string name,
                         std::size_t number_of_parameters,
                         std::function<void(std::string_view)> debug)
        : _connection(connection),
          _name(std::move(name)),
          _parameter_data(number_of_parameters),
          _parameter_pointers(number_of_parameters),
          _debug(debug)
    {
    }
    prepared_statement_t(const prepared_statement_t&) = delete;
    prepared_statement_t(prepared_statement_t&& rhs) = default;
    prepared_statement_t& operator=(const prepared_statement_t&) = delete;
    prepared_statement_t& operator=(prepared_statement_t&&) = default;
    ~prepared_statement_t() = default;

    auto* get_connection() const
    {
      return _connection;
    }

    auto& get_name() const
    {
      return _name;
    }

    auto get_number_of_parameters() const
    {
      return _parameter_pointers.size();
    }

    auto& get_parameter_data()
    {
      return _parameter_data;
    }

    auto& get_parameter_pointers()
    {
      return _parameter_pointers;
    }

    auto& get_parameter_pointers() const
    {
      return _parameter_pointers;
    }

    auto debug() const
    {
      return _debug;
    }
  };

  auto bind_parameter(prepared_statement_t& statement, const std::nullopt_t& value, int index) -> void;

  auto bind_parameter(prepared_statement_t& statement, const bool& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, const std::int32_t& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, const std::int64_t& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, const std::string& value, int index) -> void;
  auto bind_parameter(prepared_statement_t& statement, const std::string_view& value, int index) -> void;

  template <typename T>
  auto bind_parameter(prepared_statement_t& statement, const std::optional<T>& value, int index) -> void
  {
    value ? bind_parameter(statement, *value, index) : bind_parameter(statement, std::nullopt, index);
  }

}  // namespace sqlpp::postgresql

