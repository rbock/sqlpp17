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
#include <type_traits>

#include <sqlpp17/connection_base.h>
#include <sqlpp17/result.h>
#include <sqlpp17/statement.h>

#include <sqlpp17/postgresql/bool.h>
#include <sqlpp17/postgresql/char_result.h>
#include <sqlpp17/postgresql/clause.h>
#include <sqlpp17/postgresql/connection_config.h>
#include <sqlpp17/postgresql/context.h>
#include <sqlpp17/postgresql/operator.h>
#include <sqlpp17/postgresql/parameter.h>
#include <sqlpp17/postgresql/prepared_statement.h>
#include <sqlpp17/postgresql/to_sql_string.h>

namespace sqlpp::postgresql
{
  template <typename Pool, ::sqlpp::debug Debug>
  class base_connection;

  template <::sqlpp::debug Debug = ::sqlpp::debug::allowed>
  using connection_t = base_connection<no_pool, Debug>;
};  // namespace sqlpp::postgresql

namespace sqlpp::postgresql::detail
{
  class connection_cleanup_t
  {
  public:
    auto operator()(PGconn* handle) -> void
    {
      if (handle)
        PQfinish(handle);
    }
  };
  using unique_connection_ptr = std::unique_ptr<PGconn, detail::connection_cleanup_t>;

  template<typename Connection, typename Statement>
  inline auto execute(const Connection& connection, const Statement& statement) -> detail::unique_result_ptr
  {
    const auto sql_string = to_sql_string_c(context_t{}, statement);

    if (connection.is_debug_allowed())
      connection.debug("Executing: '" + sql_string + "'");

    // If one day we switch to binary format, then we could use PQexecParams with resultFormat=1
    auto result = detail::unique_result_ptr(PQexec(connection.get(), sql_string.c_str()), {});

    if (not result)
    {
      throw sqlpp::exception("Postgresql: out of memory (query was >>" + sql_string + "<<\n");
    }

    switch (PQresultStatus(result.get()))
    {
      case PGRES_COMMAND_OK:
        [[fallthrough]];
      case PGRES_TUPLES_OK:
        return result;
      default:
        throw sqlpp::exception(std::string("Postgresql: Error during query execution: ") +
                               PQresultErrorMessage(result.get()) + " (query was >>" + sql_string + "<<\n");
    }
  }

  // direct execution
  inline auto config_field_to_string(std::string_view name, const std::optional<std::string>& value) -> std::string
  {
    return value ? std::string(name) + "=" + *value + " " : "";
  }

  inline auto config_field_to_string(std::string_view name, const std::optional<int>& value) -> std::string
  {
    return value ? std::string(name) + "=" + std::to_string(*value) + " " : "";
  }

  inline auto config_field_to_string(std::string_view name, const std::optional<bool>& value) -> std::string
  {
    return value ? std::string(name) + "=" + std::to_string(*value) + " " : "";
  }

}  // namespace sqlpp::postgresql::detail

namespace sqlpp::postgresql
{
  template<typename Pool, ::sqlpp::debug Debug>
  class base_connection : public ::sqlpp::connection_base
  {
    detail::unique_connection_ptr _handle;
    Pool* _connection_pool = nullptr;
    bool _transaction_active = false;
    std::function<void(std::string_view)> _debug;

    mutable std::size_t _statement_index = 0;

    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::clause_base;

    friend Pool;

    base_connection(const connection_config_t& config,
                 detail::unique_connection_ptr&& handle,
                 Pool* connection_pool)
        : _handle(std::move(handle)), _connection_pool(connection_pool), _debug(config.debug)
    {
    }

    base_connection(const connection_config_t& config, Pool* connection_pool) : base_connection(config)
    {
      _connection_pool = connection_pool;
    }

  public:
    base_connection() = delete;
    base_connection(const connection_config_t& config) : _handle(nullptr, {}), _debug(config.debug)
    {
      if (config.pre_connect)
      {
        config.pre_connect(get());
      }

      auto conninfo = std::string{};
      conninfo += detail::config_field_to_string("host", config.host);
      conninfo += detail::config_field_to_string("hostaddr", config.hostaddr);
      conninfo += detail::config_field_to_string("port", config.port);
      conninfo += detail::config_field_to_string("dbname", config.dbname);
      conninfo += detail::config_field_to_string("user", config.user);
      conninfo += detail::config_field_to_string("password", config.password);
      conninfo += detail::config_field_to_string("passfile", config.passfile);
      conninfo += detail::config_field_to_string("connect_timeout", config.connect_timeout);
      conninfo += detail::config_field_to_string("client_encoding", config.client_encoding);
      conninfo += detail::config_field_to_string("options", config.options);
      conninfo += detail::config_field_to_string("application_name", config.application_name);
      conninfo += detail::config_field_to_string("fallback_application_name", config.fallback_application_name);
      conninfo += detail::config_field_to_string("keepalives", config.keepalives);
      conninfo += detail::config_field_to_string("keepalives_idle", config.keepalives_idle);
      conninfo += detail::config_field_to_string("keepalives_interval", config.keepalives_interval);
      conninfo += detail::config_field_to_string("keepalives_count", config.keepalives_count);
      conninfo += detail::config_field_to_string("sslmode", config.sslmode);
      conninfo += detail::config_field_to_string("sslcompression", config.sslcompression);
      conninfo += detail::config_field_to_string("sslcert", config.sslcert);
      conninfo += detail::config_field_to_string("sslkey", config.sslkey);
      conninfo += detail::config_field_to_string("sslrootcert", config.sslrootcert);
      conninfo += detail::config_field_to_string("sslcrl", config.sslcrl);
      conninfo += detail::config_field_to_string("requirepeer", config.requirepeer);
      conninfo += detail::config_field_to_string("krbsrvname", config.krbsrvname);
      conninfo += detail::config_field_to_string("gsslib", config.gsslib);
      conninfo += detail::config_field_to_string("service", config.service);
      conninfo += detail::config_field_to_string("target_session_attrs", config.target_session_attrs);

      _handle.reset(PQconnectdb(conninfo.c_str()));

      if (PQstatus(_handle.get()) != CONNECTION_OK)
      {
        throw sqlpp::exception("Postgresql: could not connect to server: " +
                               std::string(PQerrorMessage(_handle.get())));
      }

      if (config.post_connect)
      {
        config.post_connect(_handle.get());
      }
    }
    base_connection(const base_connection&) = delete;
    base_connection(base_connection&&) = default;
    base_connection& operator=(const base_connection&) = delete;
    base_connection& operator=(base_connection&&) = default;
    ~base_connection()
    {
      if constexpr (not std::is_same_v<Pool, ::sqlpp::no_pool>)
      {
        if (_connection_pool)
          _connection_pool->put(std::move(_handle));
      }
    }

    template <typename... Clauses>
    auto operator()(const ::sqlpp::statement<Clauses...>& statement)
    {
      using Statement = ::sqlpp::statement<Clauses...>;
      if constexpr (constexpr auto _check = check_statement_executable<base_connection>(type_v<Statement>); _check)
      {
        using ResultType = result_type_of_t<Statement>;
        if constexpr (std::is_same_v<ResultType, insert_result>)
        {
          return PQoidValue(detail::execute(*this, statement).get());
        }
        else if constexpr (std::is_same_v<ResultType, delete_result>)
        {
          return std::strtoll(PQcmdTuples(detail::execute(*this, statement).get()), nullptr, 10);
        }
        else if constexpr (std::is_same_v<ResultType, update_result>)
        {
          return std::strtoll(PQcmdTuples(detail::execute(*this, statement).get()), nullptr, 10);
        }
        else if constexpr (std::is_same_v<ResultType, select_result>)
        {
          auto result = detail::execute(*this, statement);

          using _result_type = char_result_t<result_row_of_t<Statement>>;
          return ::sqlpp::result_t<_result_type>{_result_type{std::move(result)}};
        }
        else if constexpr (std::is_same_v<ResultType, execute_result>)
        {
          return std::strtoll(PQcmdTuples(detail::execute(*this, statement).get()), nullptr, 10);
        }
        else
        {
          static_assert(wrong<Statement>, "Unknown statement type");
        }
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }

    template <typename... Clauses>
    auto prepare(const ::sqlpp::statement<Clauses...>& statement)
    {
      using Statement = ::sqlpp::statement<Clauses...>;
      if constexpr (constexpr auto _check = check_statement_preparable<base_connection>(type_v<Statement>); _check)
      {
        return ::sqlpp::postgresql::prepared_statement_t{*this, statement};
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }

    auto start_transaction() -> void
    {
      if (_transaction_active)
      {
        throw sqlpp::exception("Postgresql: Cannot have more than one open transaction per connection");
      }

      detail::execute(*this, "START TRANSACTION");
      _transaction_active = true;
    }

    auto commit() -> void
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("Postgresql: Cannot commit without active transaction");
      }

      _transaction_active = false;
      detail::execute(*this, "COMMIT");
    }

    auto rollback() -> void
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("Postgresql: Cannot rollback without active transaction");
      }

      _transaction_active = false;
      detail::execute(*this, "ROLLBACK");
    }

    auto destroy_transaction() noexcept -> void
    {
      try
      {
        if (is_debug_allowed())
          debug("Auto rollback!");

        rollback();
      }
      catch (...)
      {
        // This is called in ~transaction().
        // We must not throw
      }
    }

    constexpr auto is_debug_allowed() const
    {
      return Debug == ::sqlpp::debug::allowed;
    }

    auto debug([[maybe_unused]] std::string_view message) const
    {
      if (is_debug_allowed())
        _debug(message);
    }

    auto* get() const
    {
      return _handle.get();
    }

    auto is_alive() -> bool
    {
      return PQstatus(_handle.get()) == CONNECTION_OK;
    }

    auto get_statement_index() const
    {
      return ++_statement_index;
    }
  };

}  // namespace sqlpp::postgresql


