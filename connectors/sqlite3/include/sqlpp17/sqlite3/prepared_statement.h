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

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <sqlpp17/prepared_statement_parameters.h>

#include <sqlpp17/sqlite3/prepared_statement_result.h>

namespace sqlpp::sqlite3::detail
{
  inline void check_bind_result(int result, const char* const type)
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
}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  inline auto bind_parameter(::sqlite3_stmt* statement, [[maybe_unused]] const std::nullopt_t&, int index) -> void
  {
    const auto result = sqlite3_bind_null(statement, index);
    detail::check_bind_result(result, "NULL");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, bool& value, int index) -> void
  {
    const auto result = sqlite3_bind_int(statement, index, value);
    detail::check_bind_result(result, "bool");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, std::int32_t& value, int index) -> void
  {
    const auto result = sqlite3_bind_int(statement, index, value);
    detail::check_bind_result(result, "int32_t");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, std::int64_t& value, int index) -> void
  {
    const auto result = sqlite3_bind_int64(statement, index, value);
    detail::check_bind_result(result, "int64_t");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, float& value, int index) -> void
  {
    // There is no bind_float
    const auto result = sqlite3_bind_double(statement, index, value);
    detail::check_bind_result(result, "float");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, double& value, int index) -> void
  {
    const auto result = sqlite3_bind_double(statement, index, value);
    detail::check_bind_result(result, "double");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, std::string& value, int index) -> void
  {
    const auto result =
        sqlite3_bind_text(statement, index, value.data(), static_cast<int>(value.size()), SQLITE_STATIC);
    detail::check_bind_result(result, "string");
  }

  inline auto bind_parameter(::sqlite3_stmt* statement, std::string_view& value, int index) -> void
  {
    const auto result =
        sqlite3_bind_text(statement, index, value.data(), static_cast<int>(value.size()), SQLITE_STATIC);
    detail::check_bind_result(result, "string_view");
  }

  template <typename T>
  auto bind_parameter(::sqlite3_stmt* statement, std::optional<T>& value, int index) -> void
  {
    value ? bind_parameter(statement, *value, index) : bind_parameter(statement, std::nullopt, index);
  }

  template <typename... ParameterSpecs>
  auto bind_parameters(::sqlite3_stmt* statement, ::sqlpp::prepared_statement_parameters<type_vector<ParameterSpecs...>>& parameters) -> void
  {
      int index = 0;
      (..., bind_parameter(statement, static_cast<parameter_base_t<ParameterSpecs>&>(parameters)(), ++index));
  }

  template<typename ResultType, typename ParameterVector, typename ResultRow>
  class prepared_statement_t
  {
    detail::unique_prepared_statement_ptr _handle;
    detail::result_owns_statement _ownership;
    ::sqlite3* _connection;

  public:
    ::sqlpp::prepared_statement_parameters<ParameterVector> parameters = {};

    prepared_statement_t() = default;
    template <typename Connection, typename Statement>
    prepared_statement_t(const Connection& connection, const Statement& statement, detail::result_owns_statement ownership)
        : _ownership(ownership), _connection(connection.get())
    {
      const auto sql_string = to_sql_string_c(context_t{}, statement);
      if constexpr (Connection::is_debug_allowed())
      {
        connection.debug("Preparing: '" + sql_string + "'");
      }

      ::sqlite3_stmt* statement_ptr = nullptr;

      const auto rc = sqlite3_prepare_v2(connection.get(), sql_string.c_str(), static_cast<int>(sql_string.size()),
                                         &statement_ptr, nullptr);

      _handle = detail::unique_prepared_statement_ptr(statement_ptr, {true});

      if (rc != SQLITE_OK)
      {
        throw sqlpp::exception("Sqlite3: Could not prepare statement: " + std::string(sqlite3_errmsg(connection.get())) +
                               " (statement was >>" + sql_string + "<<)\n");
      }
    }

    prepared_statement_t(const prepared_statement_t&) = delete;
    prepared_statement_t(prepared_statement_t&& rhs) = default;
    prepared_statement_t& operator=(const prepared_statement_t&) = delete;
    prepared_statement_t& operator=(prepared_statement_t&&) = default;
    ~prepared_statement_t() = default;

    auto execute()
    {
      sqlite3_reset(_handle.get());

      ::sqlpp::sqlite3::bind_parameters(_handle.get(), parameters);

      switch (const auto rc = sqlite3_step(_handle.get()); rc)
      {
        case SQLITE_OK:
          [[fallthrough]];
        case SQLITE_ROW:
          [[fallthrough]];  // might occur if execute is called with a select
        case SQLITE_DONE:
          break;
        default:
          throw sqlpp::exception("Sqlite3: Could not execute statement: " + std::string(sqlite3_errstr(rc)));
      }

      if constexpr (std::is_same_v<ResultType, insert_result>)
      {
        return sqlite3_last_insert_rowid(_connection);
      }
      else if constexpr (std::is_same_v<ResultType, delete_result>)
      {
        return sqlite3_changes(_connection);
      }
      else if constexpr (std::is_same_v<ResultType, update_result>)
      {
        return sqlite3_changes(_connection);
      }
      else if constexpr (std::is_same_v<ResultType, select_result>)
      {
        return ::sqlpp::result_t<prepared_statement_result_t<ResultRow>>{
            (_ownership == (detail::result_owns_statement{true}))
                ? detail::unique_prepared_statement_ptr{_handle.release(), {true}}
                : detail::unique_prepared_statement_ptr{_handle.get(), {false}}};
      }
      else if constexpr (std::is_same_v<ResultType, execute_result>)
      {
        return sqlite3_changes(_connection);
      }
      else
      {
        static_assert(wrong<ResultType>, "Unknown statement result type");
      }
    }

    auto* get() const
    {
      return _handle.get();
    }

    auto* connection() const
    {
      return _connection;
    }
  };

  template <typename Connection, typename Statement>
  prepared_statement_t(const Connection&, const Statement&, detail::result_owns_statement)
      ->prepared_statement_t<result_type_of_t<Statement>, parameters_of_t<Statement>, result_row_of_t<Statement>>;

  template <typename ResultType, typename ParameterVector, typename ResultRow>
  auto execute(prepared_statement_t<ResultType, ParameterVector, ResultRow>& statement)
  {
    return statement.execute();
  }

}  // namespace sqlpp::sqlite3

