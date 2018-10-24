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
#include <array>

#include <sqlpp17/exception.h>
#include <sqlpp17/prepared_statement_parameters.h>
#include <sqlpp17/result.h>
#include <sqlpp17/result_row.h>

#include <sqlpp17/mysql/mysql.h>
#include <sqlpp17/mysql/prepared_statement_result.h>

namespace sqlpp::mysql::detail
{
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
  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, const std::nullopt_t& value) -> void
  {
    meta_data.is_null = true;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_NULL;
    parameter.buffer = nullptr;
    parameter.buffer_length = 0;
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  // Taking parameters by non-const reference to prevent temporaries being created which
  // would lead to dangling pointers in the implementation
  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, bool& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_TINY;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, std::int32_t& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_LONG;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, std::int64_t& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_LONGLONG;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, float& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_FLOAT;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, double& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_DOUBLE;
    parameter.buffer = &value;
    parameter.buffer_length = sizeof(value);
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, std::string& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_STRING;
    parameter.buffer = value.data();
    parameter.buffer_length = value.size();
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  inline auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, std::string_view& value) -> void
  {
    meta_data.is_null = false;

    parameter.is_null = &meta_data.is_null;
    parameter.buffer_type = MYSQL_TYPE_STRING;
    parameter.buffer = const_cast<char*>(value.data());  // Sigh...
    parameter.buffer_length = value.size();
    parameter.length = &parameter.buffer_length;
    parameter.is_unsigned = false;
    parameter.error = nullptr;
  }

  template <typename T>
  auto bind_parameter(bind_meta_data_t& meta_data, MYSQL_BIND& parameter, std::optional<T>& value) -> void
  {
    value ? bind_parameter(meta_data, parameter, *value) : bind_parameter(meta_data, parameter, std::nullopt);
  }

  template <typename... ParameterSpecs>
  auto bind_parameters(std::array<bind_meta_data_t, sizeof...(ParameterSpecs)>& meta_data,
                       std::array<MYSQL_BIND, sizeof...(ParameterSpecs)>& bind_data,
                       ::sqlpp::prepared_statement_parameters<type_vector<ParameterSpecs...>>& parameters) -> void
  {
      int index = 0;
      (..., (bind_parameter(meta_data[index], bind_data[index],
                            static_cast<parameter_base_t<ParameterSpecs>&>(parameters)()),
             ++index));
  }

  template<typename ResultType, typename ParameterVector, typename ResultRow>
  class prepared_statement_t
  {
    detail::unique_prepared_statement_ptr _handle;
#warning: This should be a tuple of correct types
    std::array<bind_meta_data_t, ParameterVector::size()> _parameter_bind_meta_data = {};
    std::array<MYSQL_BIND, ParameterVector::size()> _parameter_bind_data = {};

  public:
    ::sqlpp::prepared_statement_parameters<ParameterVector> parameters = {};

    prepared_statement_t() = default;
    template<typename Connection, typename Statement>
    prepared_statement_t(const Connection& connection, const Statement& statement)
    {
      detail::thread_init();
      const auto sql_string = to_sql_string_c(context_t{}, statement);

      if constexpr (Connection::is_debug_allowed())
        connection.debug("Preparing: '" + sql_string + "'");

      _handle = detail::unique_prepared_statement_ptr(mysql_stmt_init(connection.get()), {});
      if (not _handle)
      {
        throw sqlpp::exception("MySQL: Could not allocate prepared statement\n");
      }
      if (mysql_stmt_prepare(_handle.get(), sql_string.data(), sql_string.size()))
      {
        throw sqlpp::exception("MySQL: Could not prepare statement: " + std::string(mysql_error(connection.get())) +
                               " (statement was >>" + sql_string + "<<\n");
      }
    }
    prepared_statement_t(const prepared_statement_t&) = delete;
    prepared_statement_t(prepared_statement_t&& rhs) = default;
    prepared_statement_t& operator=(const prepared_statement_t&) = delete;
    prepared_statement_t& operator=(prepared_statement_t&&) = default;
    ~prepared_statement_t() = default;

    auto execute()
    {
      detail::thread_init();

      ::sqlpp::mysql::bind_parameters(_parameter_bind_meta_data, _parameter_bind_data, parameters);

      if (mysql_stmt_bind_param(_handle.get(), _parameter_bind_data.data()))
      {
        throw sqlpp::exception(std::string("MySQL: Could not bind parameters to statement") +
                               mysql_stmt_error(_handle.get()));
      }

      if (mysql_stmt_execute(_handle.get()))
      {
        throw sqlpp::exception(std::string("MySQL: Could not execute prepared statement: ") +
                               mysql_stmt_error(_handle.get()));
      }

      if constexpr (std::is_same_v<ResultType, insert_result>)
      {
        return mysql_stmt_insert_id(this->get());
      }
      else if constexpr (std::is_same_v<ResultType, delete_result>)
      {
        return mysql_stmt_affected_rows(this->get());
      }
      else if constexpr (std::is_same_v<ResultType, update_result>)
      {
        return mysql_stmt_affected_rows(this->get());
      }
      else if constexpr (std::is_same_v<ResultType, select_result>)
      {
        mysql_stmt_store_result(this->get());

        return ::sqlpp::result_t<prepared_statement_result_t<ResultRow>>{{detail::unique_prepared_result_ptr{_handle.get(), {}}, column_count_v<ResultRow>}};
      }
      else if constexpr (std::is_same_v<ResultType, execute_result>)
      {
        return mysql_stmt_affected_rows(this->get());
      }
      else
      {
        static_assert(wrong<ResultType>, "Unknown statement result type");
      }
    }

    auto get() const -> MYSQL_STMT*
    {
      return _handle.get();
    }
  };

  template <typename Connection, typename Statement>
  prepared_statement_t(const Connection&, const Statement&)->prepared_statement_t<result_type_of_t<Statement>, parameters_of_t<Statement>, result_row_of_t<Statement>>;

  template<typename ResultType, typename ParameterVector, typename ResultRow>
  auto execute(prepared_statement_t<ResultType, ParameterVector, ResultRow>& statement)
  {
    return statement.execute();
  }

}  // namespace sqlpp::mysql

