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

#include <sqlpp17/core/connection.h>
#include <sqlpp17/core/result.h>
#include <sqlpp17/core/statement.h>

#include <sqlpp17/mysql/clause.h>
#include <sqlpp17/mysql/connection_config.h>
#include <sqlpp17/mysql/context.h>
#include <sqlpp17/mysql/direct_execution_result.h>
#include <sqlpp17/mysql/mysql.h>
#include <sqlpp17/mysql/prepared_statement.h>
#include <sqlpp17/mysql/prepared_statement_result.h>

namespace sqlpp::mysql
{
  template <typename Pool, ::sqlpp::debug Debug>
  class base_connection;

  template <::sqlpp::debug Debug = ::sqlpp::debug::allowed>
  using connection_t = base_connection<no_pool, Debug>;

};  // namespace sqlpp::mysql

namespace sqlpp::mysql::detail
{
  class connection_cleanup_t
  {
  public:
    auto operator()(MYSQL* handle) const noexcept -> void
    {
      if (handle)
      {
        mysql_close(handle);
      }
    }
  };
  using unique_connection_ptr = std::unique_ptr<MYSQL, detail::connection_cleanup_t>;

  template <typename Pool, ::sqlpp::debug Debug>
  inline auto execute_query(const base_connection<Pool, Debug>& connection, const std::string& query) -> void
  {
    detail::thread_init();

    if constexpr (base_connection<Pool, Debug>::is_debug_allowed())
      connection.debug("Executing: '" + query + "'");

    if (mysql_real_query(connection.get(), query.c_str(), query.size()))
    {
      throw sqlpp::exception("MySQL: Could not execute query: " + std::string(mysql_error(connection.get())) +
                             " (query was >>" + query + "<<\n");
    }
  }

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  void global_library_init(int argc = 0, char** argv = nullptr, char** groups = nullptr)
  {
    static const auto global_init_and_end = detail::scoped_library_initializer_t(argc, argv, groups);
  }

  template <typename Pool, ::sqlpp::debug Debug>
  class base_connection : public ::sqlpp::connection,
                          private ::sqlpp::pool_base<Pool>,
                          private ::sqlpp::debug_base<Debug>
  {
    using _pool_base = ::sqlpp::pool_base<Pool>;
    using _debug_base = ::sqlpp::debug_base<Debug>;

    detail::unique_connection_ptr _handle;
    bool _transaction_active = false;

    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::clause_base;

    friend Pool;

    base_connection(const connection_config_t& config, detail::unique_connection_ptr&& handle, Pool* connection_pool)
        : _pool_base{connection_pool}, _debug_base{config.debug}, _handle{std::move(handle)}
    {
    }

    base_connection(const connection_config_t& config, Pool* connection_pool) : base_connection{config}
    {
      this->_connection_pool = connection_pool;
    }

  public:
    base_connection() = delete;
    base_connection(const connection_config_t& config) : _debug_base{config.debug}, _handle(mysql_init(nullptr))
    {
      if (not _handle)
      {
        throw sqlpp::exception("MySQL: could not init mysql data structure");
      }

      if (config.pre_connect)
      {
        config.pre_connect(get());
      }

      if (config.ssl)
      {
        const auto& ssl = config.ssl.value();
        mysql_ssl_set(_handle.get(), ssl.key.c_str(), ssl.cert.c_str(), ssl.ca.empty() ? nullptr : ssl.ca.c_str(),
                      ssl.caPath.empty() ? nullptr : ssl.caPath.c_str(),
                      ssl.cipher.empty() ? nullptr : ssl.cipher.c_str());
      }

      if (!mysql_real_connect(_handle.get(), config.host.empty() ? nullptr : config.host.c_str(),
                              config.user.empty() ? nullptr : config.user.c_str(),
                              config.password.empty() ? nullptr : config.password.c_str(), nullptr, config.port,
                              config.unix_socket.empty() ? nullptr : config.unix_socket.c_str(), config.client_flag))
      {
        throw sqlpp::exception("MySQL: could not connect to server: " + std::string(mysql_error(_handle.get())));
      }

      if (mysql_set_character_set(_handle.get(), config.charset.c_str()))
      {
        throw sqlpp::exception("MySQL: can't set character set " + config.charset);
      }

      if (mysql_select_db(_handle.get(), config.database.c_str()))
      {
        throw sqlpp::exception("MySQL: can't select database '" + config.database + "'");
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
      if constexpr (not std::is_same_v<Pool, no_pool>)
      {
        if (this->_connection_pool)
          this->_connection_pool->put(std::move(_handle));
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
          return insert(statement);
        }
        else if constexpr (std::is_same_v<ResultType, delete_result>)
        {
          return delete_from(statement);
        }
        else if constexpr (std::is_same_v<ResultType, update_result>)
        {
          return update(statement);
        }
        else if constexpr (std::is_same_v<ResultType, select_result>)
        {
          return select(statement);
        }
        else if constexpr (std::is_same_v<ResultType, execute_result>)
        {
          return execute(statement);
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
        return ::sqlpp::mysql::prepared_statement_t{*this, statement};
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
        throw sqlpp::exception("MySQL: Cannot have more than one open transaction per connection");
      }

      detail::execute_query(*this, "START TRANSACTION");
      _transaction_active = true;
    }

    auto commit() -> void
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("MySQL: Cannot commit without active transaction");
      }

      _transaction_active = false;
      detail::execute_query(*this, "COMMIT");
    }

    auto rollback() -> void
    {
      if (not _transaction_active)
      {
        throw sqlpp::exception("MySQL: Cannot rollback without active transaction");
      }

      _transaction_active = false;
      detail::execute_query(*this, "ROLLBACK");
    }

    auto destroy_transaction() noexcept -> void
    {
      try
      {
        if constexpr (is_debug_allowed())
          debug("Auto rollback!");

        rollback();
      }
      catch (...)
      {
        // This is called in ~transaction().
        // We must not throw
      }
    }

    static constexpr auto is_debug_allowed()
    {
      return Debug == ::sqlpp::debug::allowed;
    }

    auto debug([[maybe_unused]] const std::string_view message) const
    {
      if constexpr (is_debug_allowed())
      {
        if (this->_debug)
          this->_debug(message);
      }
    }

    auto get() const -> MYSQL*
    {
      return _handle.get();
    }

    auto is_alive() -> bool
    {
      return mysql_ping(_handle.get()) == 0;
    }

  private:
    template <typename... Clauses>
    auto execute(const ::sqlpp::statement<Clauses...>& statement)
    {
      return detail::execute_query(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    auto insert(const Statement& statement)
    {
      this->execute(statement);

      return mysql_insert_id(this->get());
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
      this->execute(statement);
      return mysql_affected_rows(this->get());
    }

    template <typename Statement>
    auto delete_from(const Statement& statement)
    {
      this->execute(statement);
      return mysql_affected_rows(this->get());
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
      this->execute(statement);
      auto result_handle = detail::unique_result_ptr(mysql_store_result(this->get()), {});
      if (!result_handle)
      {
        throw sqlpp::exception("MySQL: Could not store result set: " + std::string(mysql_error(this->get())));
      }

      using _result_type = direct_execution_result_t<result_row_of_t<Statement>>;
      return ::sqlpp::result_t<_result_type>{_result_type{std::move(result_handle)}};
    }
  };

}  // namespace sqlpp::mysql
