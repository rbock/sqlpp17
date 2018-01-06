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

#include <sqlpp17/exception.h>

#include <sqlpp17/postgresql/char_result.h>
#include <sqlpp17/postgresql/connection.h>
#include <sqlpp17/postgresql/connection_pool.h>

namespace sqlpp::postgresql::detail
{
  auto execute_query(const connection_t& connection, const std::string& query) -> detail::unique_result_ptr
  {
    if (connection.debug())
      connection.debug()("Executing: '" + query + "'");

    // If one day we switch to binary format, then we could use PQexecParams with resultFormat=1
    auto result = detail::unique_result_ptr(PQexec(connection.get(), query.c_str()), {});

    if (not result)
    {
      throw sqlpp::exception("Postgresql: out of memory (query was >>" + query + "<<\n");
    }

    switch (PQresultStatus(result.get()))
    {
      case PGRES_COMMAND_OK:
        [[fallthrough]];
      case PGRES_TUPLES_OK:
        return result;
      default:
        throw sqlpp::exception(std::string("Postgresql: Error during query execution: ") +
                               PQresultErrorMessage(result.get()) + " (query was >>" + query + "<<\n");
    }
  }

  auto select(const connection_t& connection, const std::string& query) -> char_result_t
  {
    return {execute_query(connection, query), connection.debug()};
  }

  auto insert(const connection_t& connection, const std::string& query) -> size_t
  {
    return PQoidValue(execute_query(connection, query).get());
  }

  auto update(const connection_t& connection, const std::string& statement) -> size_t
  {
    return std::strtoll(PQcmdTuples(execute_query(connection, statement).get()), nullptr, 10);
  }

  auto delete_from(const connection_t& connection, const std::string& statement) -> size_t
  {
    return std::strtoll(PQcmdTuples(execute_query(connection, statement).get()), nullptr, 10);
  }

  auto execute(const connection_t& connection, const std::string& statement) -> void
  {
    execute_query(connection, statement);
  }

  auto prepare(const connection_t& connection,
               const std::string& statement,
               size_t no_of_parameters,
               size_t no_of_columns) -> ::sqlpp::postgresql::prepared_statement_t
  {
    const auto statement_name =
        std::to_string(connection.get_statement_index()) + "at" + std::to_string(::time(nullptr));

    if (connection.debug())
      connection.debug()("Preparing" + statement_name + ": '" + statement + "'");

    auto result = detail::unique_result_ptr(
        PQprepare(connection.get(), statement_name.c_str(), statement.c_str(), no_of_parameters, nullptr), {});

    if (not result)
    {
      throw sqlpp::exception("Postgresql: out of memory (query was >>" + statement + "<<\n");
    }

    switch (PQresultStatus(result.get()))
    {
      case PGRES_COMMAND_OK:
        [[fallthrough]];
      case PGRES_TUPLES_OK:
        return {connection.get(), statement_name, no_of_parameters, connection.debug()};
      default:
        throw sqlpp::exception(std::string("Postgresql: Error during query preparation: ") +
                               PQresultErrorMessage(result.get()) + " (query was >>" + statement + "<<\n");
    }
  }

  auto execute_prepared_statement(::sqlpp::postgresql::prepared_statement_t& prepared_statement)
      -> detail::unique_result_ptr
  {
    if (prepared_statement.debug())
      prepared_statement.debug()("Executing prepared_statement " + prepared_statement.get_name());

    auto result = detail::unique_result_ptr(
        PQexecPrepared(prepared_statement.get_connection(), prepared_statement.get_name().c_str(),
                       prepared_statement.get_number_of_parameters(),
                       prepared_statement.get_parameter_pointers().data(), nullptr, nullptr, 0),
        {});

    if (not result)
    {
      throw sqlpp::exception("Postgresql: out of memory (prepared statement " + prepared_statement.get_name() + "\n");
    }

    switch (PQresultStatus(result.get()))
    {
      case PGRES_COMMAND_OK:
        [[fallthrough]];
      case PGRES_TUPLES_OK:
        return result;
      default:
        throw sqlpp::exception(std::string("Postgresql: Error during prepared statement execution: ") +
                               PQresultErrorMessage(result.get()) + " (statement name " +
                               prepared_statement.get_name() + ")\n");
    }
  }

  auto prepared_select_t::run() -> char_result_t
  {
    return {execute_prepared_statement(_statement), _statement.debug()};
  }

  auto prepared_insert_t::run() -> size_t
  {
    return PQoidValue(execute_prepared_statement(_statement).get());
  }

  auto prepared_update_t::run() -> size_t
  {
    return std::strtoll(PQcmdTuples(execute_prepared_statement(_statement).get()), nullptr, 10);
  }

  auto prepared_delete_from_t::run() -> size_t
  {
    return std::strtoll(PQcmdTuples(execute_prepared_statement(_statement).get()), nullptr, 10);
  }

}  // namespace sqlpp::postgresql::detail

namespace sqlpp::postgresql
{
  namespace
  {
    auto config_field_to_string(std::string_view name, const std::optional<std::string>& value) -> std::string
    {
      return value ? std::string(name) + "=" + *value + " " : "";
    }

    auto config_field_to_string(std::string_view name, const std::optional<int>& value) -> std::string
    {
      return value ? std::string(name) + "=" + std::to_string(*value) + " " : "";
    }

    auto config_field_to_string(std::string_view name, const std::optional<bool>& value) -> std::string
    {
      return value ? std::string(name) + "=" + std::to_string(*value) + " " : "";
    }

  }  // namespace

  connection_t::connection_t(const connection_config_t& config) : _handle(nullptr, {}), _debug(config.debug)
  {
    if (config.pre_connect)
    {
      config.pre_connect(get());
    }

    auto conninfo = std::string{};
    conninfo += config_field_to_string("host", config.host);
    conninfo += config_field_to_string("hostaddr", config.hostaddr);
    conninfo += config_field_to_string("port", config.port);
    conninfo += config_field_to_string("dbname", config.dbname);
    conninfo += config_field_to_string("user", config.user);
    conninfo += config_field_to_string("password", config.password);
    conninfo += config_field_to_string("passfile", config.passfile);
    conninfo += config_field_to_string("connect_timeout", config.connect_timeout);
    conninfo += config_field_to_string("client_encoding", config.client_encoding);
    conninfo += config_field_to_string("options", config.options);
    conninfo += config_field_to_string("application_name", config.application_name);
    conninfo += config_field_to_string("fallback_application_name", config.fallback_application_name);
    conninfo += config_field_to_string("keepalives", config.keepalives);
    conninfo += config_field_to_string("keepalives_idle", config.keepalives_idle);
    conninfo += config_field_to_string("keepalives_interval", config.keepalives_interval);
    conninfo += config_field_to_string("keepalives_count", config.keepalives_count);
    conninfo += config_field_to_string("sslmode", config.sslmode);
    conninfo += config_field_to_string("sslcompression", config.sslcompression);
    conninfo += config_field_to_string("sslcert", config.sslcert);
    conninfo += config_field_to_string("sslkey", config.sslkey);
    conninfo += config_field_to_string("sslrootcert", config.sslrootcert);
    conninfo += config_field_to_string("sslcrl", config.sslcrl);
    conninfo += config_field_to_string("requirepeer", config.requirepeer);
    conninfo += config_field_to_string("krbsrvname", config.krbsrvname);
    conninfo += config_field_to_string("gsslib", config.gsslib);
    conninfo += config_field_to_string("service", config.service);
    conninfo += config_field_to_string("target_session_attrs", config.target_session_attrs);

    _handle.reset(PQconnectdb(conninfo.c_str()));

    if (PQstatus(_handle.get()) != CONNECTION_OK)
    {
      throw sqlpp::exception("Postgresql: could not connect to server: " + std::string(PQerrorMessage(_handle.get())));
    }

    if (config.post_connect)
    {
      config.post_connect(_handle.get());
    }
  }

  auto connection_t::is_alive() -> bool
  {
    return PQstatus(_handle.get()) == CONNECTION_OK;
  }

  connection_t::~connection_t()
  {
    if (_connection_pool)
    {
      _connection_pool->put(std::move(_handle));
    }
  }
}  // namespace sqlpp::postgresql

