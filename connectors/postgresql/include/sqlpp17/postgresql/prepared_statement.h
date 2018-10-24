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

#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <libpq-fe.h>

#include <sqlpp17/prepared_statement_parameters.h>

namespace sqlpp::postgresql
{
  struct prepared_statement_cleanup_t
  {
    std::string_view _name;
  public:
    auto operator()(PGconn* handle) -> void
    {
#warning implement cleanup
    }
  };
  using unique_prepared_statement_ptr = std::unique_ptr<PGconn, prepared_statement_cleanup_t>;

  inline auto bind_parameter([[maybe_unused]] std::string& parameter_string, char*& parameter_pointer, const std::nullopt_t& value) -> void
  {
    parameter_pointer = nullptr;
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, bool& value) -> void
  {
    parameter_string = value ? "TRUE" : "FALSE";
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, std::int32_t& value) -> void
  {
    parameter_string = std::to_string(value);
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, std::int64_t& value) -> void
  {
    parameter_string = std::to_string(value);
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, float& value) -> void
  {
    parameter_string = to_sql_string_c(::sqlpp::postgresql::context_t{}, value);
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, double& value) -> void
  {
    parameter_string = to_sql_string_c(::sqlpp::postgresql::context_t{}, value);
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, std::string& value) -> void
  {
    parameter_string = value;
    parameter_pointer = parameter_string.data();
  }

  inline auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, std::string_view& value) -> void
  {
    parameter_string = value;
    parameter_pointer = parameter_string.data();
  }

  template <typename T>
  auto bind_parameter(std::string& parameter_string, char*& parameter_pointer, std::optional<T>& value) -> void
  {
    value ? bind_parameter(parameter_string, parameter_pointer, *value) : bind_parameter(parameter_string, parameter_pointer, std::nullopt);
  }

  template <typename... ParameterSpecs>
  auto bind_parameters(std::array<std::string, sizeof...(ParameterSpecs)>& parameter_strings,
                       std::array<char*, sizeof...(ParameterSpecs)>& parameter_pointers,
                       ::sqlpp::prepared_statement_parameters<type_vector<ParameterSpecs...>>& parameters) -> void
  {
    int index = 0;
    (..., (bind_parameter(parameter_strings[index], parameter_pointers[index],
                          static_cast<parameter_base_t<ParameterSpecs>&>(parameters)()),
           ++index));
  }

  /* PGprepare CAN be informed about the nature of parameters using OIDs from pg_type.h
     e.g. TEXTOID or INT4OID
     However, it seems easier to pass type information in the query via $1:bigint for
     example. See https://www.postgresql.org/docs/10/static/libpq-exec.html
     Caveat: We need to store all parameters as strings. And postgresql then has to
             parse those strings.
  */
  template<typename ResultType, typename ParameterVector, typename ResultRow>
  class prepared_statement_t
  {
    std::string _name;
    unique_prepared_statement_ptr _connection;

    std::array<std::string, ParameterVector::size()> _parameter_strings;
    std::array<char*, ParameterVector::size()> _parameter_pointers;

  public:
    ::sqlpp::prepared_statement_parameters<ParameterVector> parameters = {};

    prepared_statement_t() = default;
    template <typename Connection, typename Statement>
    prepared_statement_t(const Connection& connection, const Statement& statement)
        : _name(std::to_string(connection.get_statement_index()) + "at" + std::to_string(::time(nullptr))),
          _connection(connection.get(), {_name})
    {
      const auto sql_string = to_sql_string_c(context_t{}, statement);
#warning: connection needs to decide whether or not to print
      connection.debug("Preparing " + _name + ": '" + sql_string + "'");

      auto result = detail::unique_result_ptr(
          PQprepare(connection.get(), _name.c_str(), sql_string.c_str(), ParameterVector::size(), nullptr), {});

      if (not result)
      {
        throw sqlpp::exception("Postgresql: out of memory (query was >>" + sql_string + "<<\n");
      }

      switch (PQresultStatus(result.get()))
      {
        case PGRES_COMMAND_OK:
          [[fallthrough]];
        case PGRES_TUPLES_OK:
          break;
        default:
          throw sqlpp::exception(std::string("Postgresql: Error during query preparation: ") +
                                 PQresultErrorMessage(result.get()) + " (query was >>" + sql_string + "<<\n");
      }
    }
    prepared_statement_t(const prepared_statement_t&) = delete;
    prepared_statement_t(prepared_statement_t&& rhs) = default;
    prepared_statement_t& operator=(const prepared_statement_t&) = delete;
    prepared_statement_t& operator=(prepared_statement_t&&) = default;
    ~prepared_statement_t() = default;

    auto execute()
    {
      ::sqlpp::postgresql::bind_parameters(_parameter_strings, _parameter_pointers, parameters);
      auto result = detail::unique_result_ptr(
          PQexecPrepared(_connection.get(), _name.c_str(),
                         _parameter_pointers.size(),
                         _parameter_pointers.data(), nullptr, nullptr, 0),
          {});

      if (not result)
      {
        throw sqlpp::exception("Postgresql: out of memory (prepared statement " + _name + "\n");
      }

      switch (PQresultStatus(result.get()))
      {
        case PGRES_COMMAND_OK:
          [[fallthrough]];
        case PGRES_TUPLES_OK:
          break;
        default:
          throw sqlpp::exception(std::string("Postgresql: Error during prepared statement execution: ") +
                                 PQresultErrorMessage(result.get()) + " (statement name " +
                                 _name + ")\n");
      }

      if constexpr (std::is_same_v<ResultType, insert_result>)
      {
        return PQoidValue(result.get());
      }
      else if constexpr (std::is_same_v<ResultType, delete_result>)
      {
        return std::strtoll(PQcmdTuples(result.get()), nullptr, 10);
      }
      else if constexpr (std::is_same_v<ResultType, update_result>)
      {
        return std::strtoll(PQcmdTuples(result.get()), nullptr, 10);
      }
      else if constexpr (std::is_same_v<ResultType, select_result>)
      {
        return ::sqlpp::result_t<char_result_t<ResultRow>>{std::move(result)};
      }
      else if constexpr (std::is_same_v<ResultType, execute_result>)
      {
        return std::strtoll(PQcmdTuples(result.get()), nullptr, 10);
      }
      else
      {
        static_assert(wrong<ResultType>, "Unknown statement result type");
      }
    }

    auto* get_connection() const
    {
      return _connection.get();
    }

    auto& get_name() const
    {
      return _name;
    }

    auto get_number_of_parameters() const
    {
      return _parameter_pointers.size();
    }

    auto& get_parameter_strings()
    {
      return _parameter_strings;
    }

    auto& get_parameter_pointers()
    {
      return _parameter_pointers;
    }

    auto& get_parameter_pointers() const
    {
      return _parameter_pointers;
    }
  };

  template <typename Connection, typename Statement>
  prepared_statement_t(const Connection&, const Statement&)
      ->prepared_statement_t<result_type_of_t<Statement>, parameters_of_t<Statement>, result_row_of_t<Statement>>;

  template <typename ResultType, typename ParameterVector, typename ResultRow>
  auto execute(prepared_statement_t<ResultType, ParameterVector, ResultRow>& statement)
  {
    return statement.execute();
  }

}  // namespace sqlpp::postgresql

