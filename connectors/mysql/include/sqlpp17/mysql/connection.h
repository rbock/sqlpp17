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
#include <sqlpp17/exception.h>
#include <sqlpp17/result.h>
#include <sqlpp17/statement.h>

#include <sqlpp17/mysql/clause.h>
#include <sqlpp17/mysql/connection_config.h>
#include <sqlpp17/mysql/context.h>
#include <sqlpp17/mysql/direct_execution_result.h>
#include <sqlpp17/mysql/prepared_statement.h>
#include <sqlpp17/mysql/prepared_statement_result.h>

namespace sqlpp::mysql::detail
{
  class scoped_library_initializer_t
  {
  public:
    scoped_library_initializer_t(int argc, char** argv, char** groups)
    {
      mysql_library_init(argc, argv, groups);
    }

    ~scoped_library_initializer_t()
    {
      mysql_library_end();
    }
  };

  struct MySqlThreadInitializer
  {
    MySqlThreadInitializer()
    {
      if (!mysql_thread_safe())
      {
        throw sqlpp::exception("MySQL: Operating on a non-threadsafe client");
      }
      mysql_thread_init();
    }

    ~MySqlThreadInitializer()
    {
      mysql_thread_end();
    }
  };

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  enum class debug
  {
    none,
    allowed
  };

  struct no_pool
  {
  };

  template <typename Pool, debug Debug>
  class base_connection;

  template<debug Debug = debug::allowed>
  using connection_t = base_connection<no_pool, Debug>;

};  // namespace sqlpp::mysql

namespace sqlpp::mysql::detail
{
  class connection_cleanup_t
  {
  public:
    auto operator()(MYSQL* handle) -> void
    {
      mysql_close(handle);
    }
  };
  using unique_connection_ptr = std::unique_ptr<MYSQL, detail::connection_cleanup_t>;

#ifdef __APPLE__
  boost::thread_specific_ptr<MySqlThreadInitializer> mysqlThreadInit;
  inline auto thread_init() -> void
  {
    if (!mysqlThreadInit.get())
    {
      mysqlThreadInit.reset(new MySqlThreadInitializer);
    }
  }
#else
  inline auto thread_init() -> void
  {
    thread_local MySqlThreadInitializer threadInitializer;
  }
#endif

  template <typename Pool, debug Debug>
  inline auto execute_query(const base_connection<Pool, Debug>& connection, const std::string& query) -> void
  {
    detail::thread_init();

    connection.debug("Executing: '" + query + "'");

    if (mysql_real_query(connection.get(), query.c_str(), query.size()))
    {
      throw sqlpp::exception("MySQL: Could not execute query: " + std::string(mysql_error(connection.get())) +
                             " (query was >>" + query + "<<\n");
    }
  }

  template <typename Pool, debug Debug>
  inline auto prepare(const base_connection<Pool, Debug>& connection,
                      const std::string& statement,
                      size_t no_of_parameters,
                      size_t no_of_columns) -> ::sqlpp::mysql::prepared_statement_t
  {
    thread_init();

    connection.debug("Preparing: '" + statement + "'");

    auto statement_handle = detail::unique_prepared_statement_ptr(mysql_stmt_init(connection.get()), {});
    if (not statement_handle)
    {
      throw sqlpp::exception("MySQL: Could not allocate prepared statement\n");
    }
    if (mysql_stmt_prepare(statement_handle.get(), statement.data(), statement.size()))
    {
      throw sqlpp::exception("MySQL: Could not prepare statement: " + std::string(mysql_error(connection.get())) +
                             " (statement was >>" + statement + "<<\n");
    }

    return {std::move(statement_handle), no_of_parameters, no_of_columns};
  }

  inline auto execute_prepared_statement(::sqlpp::mysql::prepared_statement_t& prepared_statement) -> void
  {
    thread_init();

    if (mysql_stmt_bind_param(prepared_statement.get(), prepared_statement.get_bind_data().data()))
    {
      throw sqlpp::exception(std::string("MySQL: Could not bind parameters to statement") +
                             mysql_stmt_error(prepared_statement.get()));
    }

    if (mysql_stmt_execute(prepared_statement.get()))
    {
      throw sqlpp::exception(std::string("MySQL: Could not execute prepared statement: ") +
                             mysql_stmt_error(prepared_statement.get()));
    }
  }

  class prepared_select_t : public ::sqlpp::mysql::prepared_statement_t
  {
  public:
    prepared_select_t(::sqlpp::mysql::prepared_statement_t&& statement)
        : ::sqlpp::mysql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> prepared_statement_result_t;
  };

  class prepared_insert_t : public ::sqlpp::mysql::prepared_statement_t
  {
  public:
    prepared_insert_t(::sqlpp::mysql::prepared_statement_t&& statement)
        : ::sqlpp::mysql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_update_t : public ::sqlpp::mysql::prepared_statement_t
  {
  public:
    prepared_update_t(::sqlpp::mysql::prepared_statement_t&& statement)
        : ::sqlpp::mysql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_delete_from_t : public ::sqlpp::mysql::prepared_statement_t
  {
  public:
    prepared_delete_from_t(::sqlpp::mysql::prepared_statement_t&& statement)
        : ::sqlpp::mysql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  void global_library_init(int argc = 0, char** argv = nullptr, char** groups = nullptr)
  {
    static const auto global_init_and_end = detail::scoped_library_initializer_t(argc, argv, groups);
  }

  template <typename Pool, debug Debug>
  class base_connection : public ::sqlpp::connection_base
  {
    detail::unique_connection_ptr _handle;
    bool _transaction_active = false;
    std::function<void(std::string_view)> _debug;
    Pool* _pool = nullptr;

    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::clause_base;

  public:
    base_connection() = delete;
    base_connection(const connection_config_t& config)
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
        if (_pool)
          _pool->put(std::move(_handle));
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
        using ResultType = result_type_of_t<Statement>;
        if constexpr (std::is_same_v<ResultType, insert_result>)
        {
          return prepare_insert(statement);
        }
        else if constexpr (std::is_same_v<ResultType, delete_result>)
        {
          return prepare_delete_from(statement);
        }
        else if constexpr (std::is_same_v<ResultType, update_result>)
        {
          return prepare_update(statement);
        }
        else if constexpr (std::is_same_v<ResultType, select_result>)
        {
          return prepare_select(statement);
        }
        else if constexpr (std::is_same_v<ResultType, execute_result>)
        {
          return prepare_execute(statement);
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
        debug("Auto rollback!");

        rollback();
      }
      catch (...)
      {
        // This is called in ~transaction().
        // We must not throw
      }
    }

    auto debug([[maybe_unused]] std::string_view message) const
    {
      if constexpr (Debug == debug::allowed)
        _debug(message);
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
    [[nodiscard]] auto prepare_insert(const Statement& statement)
    {
      return ::sqlpp::prepared_statement_t{
          statement, detail::prepared_insert_t{detail::prepare(*this, to_sql_string_c(context_t{}, statement),
                                                               parameters_of_t<Statement>::size(), 0)}};
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
      this->execute(statement);
      return mysql_affected_rows(this->get());
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_update(const Statement& statement)
    {
      return ::sqlpp::prepared_statement_t{
          statement, detail::prepared_update_t{detail::prepare(*this, to_sql_string_c(context_t{}, statement),
                                                               parameters_of_t<Statement>::size(), 0)}};
    }

    template <typename Statement>
    auto delete_from(const Statement& statement)
    {
      this->execute(statement);
      return mysql_affected_rows(this->get());
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_delete_from(const Statement& statement)
    {
      return ::sqlpp::prepared_statement_t{
          statement, detail::prepared_delete_from_t{detail::prepare(*this, to_sql_string_c(context_t{}, statement),
                                                                    parameters_of_t<Statement>::size(), 0)}};
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

      return ::sqlpp::result_t<result_row_of_t<Statement>, direct_execution_result_t>{
          direct_execution_result_t{std::move(result_handle)}};
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_select(const Statement& statement)
    {
      return ::sqlpp::prepared_statement_t{
          statement, detail::prepared_select_t{detail::prepare(*this, to_sql_string_c(context_t{}, statement),
                                                               parameters_of_t<Statement>::size(),
                                                               get_no_of_result_columns(statement))}};
    }
  };

}  // namespace sqlpp::mysql

namespace sqlpp::mysql::detail
{
  inline auto prepared_select_t::run() -> prepared_statement_result_t
  {
    execute_prepared_statement(*this);
    return {*this};
  }

  inline auto prepared_insert_t::run() -> size_t
  {
    execute_prepared_statement(*this);
    return mysql_stmt_insert_id(this->get());
  }

  inline auto prepared_update_t::run() -> size_t
  {
    execute_prepared_statement(*this);
    return mysql_stmt_affected_rows(this->get());
  }

  inline auto prepared_delete_from_t::run() -> size_t
  {
    execute_prepared_statement(*this);
    return mysql_stmt_affected_rows(this->get());
  }

}  // namespace sqlpp::mysql::detail

