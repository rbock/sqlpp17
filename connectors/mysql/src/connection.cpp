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

#include <sqlpp17/mysql/bind_result.h>
#include <sqlpp17/mysql/char_result.h>
#include <sqlpp17/mysql/connection.h>

namespace
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

#ifdef __APPLE__
  boost::thread_specific_ptr<MySqlThreadInitializer> mysqlThreadInit;
  void thread_init()
  {
    if (!mysqlThreadInit.get())
    {
      mysqlThreadInit.reset(new MySqlThreadInitializer);
    }
  }
#else
  void thread_init()
  {
    thread_local MySqlThreadInitializer threadInitializer;
  }
#endif

}  // namespace

namespace sqlpp::mysql::detail
{
  auto connection_cleanup(MYSQL* handle) -> void
  {
    mysql_close(handle);
  }

  auto result_cleanup(MYSQL_RES* result) -> void
  {
    mysql_free_result(result);
  }

  auto statement_cleanup(MYSQL_STMT* statement) -> void
  {
    mysql_stmt_close(statement);
  }

  auto execute_query(const connection_t& connection, const std::string& query) -> void
  {
    thread_init();

    if (connection.debug())
      connection.debug()("Executing: '" + query + "'");

    if (mysql_real_query(connection.get(), query.c_str(), query.size()))
    {
      throw sqlpp::exception("MySQL: Could not execute query: " + std::string(mysql_error(connection.get())) +
                             " (query was >>" + query + "<<\n");
    }
  }

  auto select(const connection_t& connection, const std::string& query) -> char_result_t
  {
    execute_query(connection, query);
    std::unique_ptr<MYSQL_RES, void (*)(MYSQL_RES*)> result_handle(mysql_store_result(connection.get()),
                                                                   result_cleanup);
    if (!result_handle)
    {
      throw sqlpp::exception("MySQL: Could not store result set: " + std::string(mysql_error(connection.get())));
    }

    return {std::move(result_handle), connection.debug()};
  }

  auto insert(const connection_t& connection, const std::string& query) -> size_t
  {
    execute_query(connection, query);

    return mysql_insert_id(connection.get());
  }

  auto update(const connection_t& connection, const std::string& statement) -> size_t
  {
    execute_query(connection, statement);
    return mysql_affected_rows(connection.get());
  }

  auto erase(const connection_t& connection, const std::string& statement) -> size_t
  {
    execute_query(connection, statement);
    return mysql_affected_rows(connection.get());
  }

  auto execute(const connection_t& connection, const std::string& statement) -> void
  {
    execute_query(connection, statement);
  }

  auto prepare(const connection_t& connection,
               const std::string& statement,
               size_t no_of_parameters,
               size_t no_of_columns) -> prepared_statement_t
  {
    thread_init();

    if (connection.debug())
      connection.debug()("Preparing: '" + statement + "'");

    auto statement_handle =
        std::unique_ptr<MYSQL_STMT, void (*)(MYSQL_STMT*)>(mysql_stmt_init(connection.get()), statement_cleanup);
    if (not statement_handle)
    {
      throw sqlpp::exception("MySQL: Could not allocate prepared statement\n");
    }
    if (mysql_stmt_prepare(statement_handle.get(), statement.data(), statement.size()))
    {
      throw sqlpp::exception("MySQL: Could not prepare statement: " + std::string(mysql_error(connection.get())) +
                             " (statement was >>" + statement + "<<\n");
    }

    return {std::move(statement_handle), no_of_parameters, no_of_columns, connection.debug()};
  }

  auto execute_prepared_statement(prepared_statement_t& prepared_statement) -> void
  {
    thread_init();

    if (prepared_statement.debug())
      prepared_statement.debug()("Executing prepared_statement");

    if (mysql_stmt_bind_param(prepared_statement.get(), prepared_statement.get_bind_data()))
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

  auto prepared_select_t::run() -> bind_result_t
  {
    execute_prepared_statement(_statement);
    return {_statement.get()};
  }

  auto prepared_insert_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return mysql_stmt_insert_id(_statement.get());
  }

  auto prepared_update_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return mysql_stmt_affected_rows(_statement.get());
  }

  auto prepared_erase_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return mysql_stmt_affected_rows(_statement.get());
  }

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  void global_library_init(int argc, char** argv, char** groups)
  {
    static const auto global_init_and_end = scoped_library_initializer_t(argc, argv, groups);
  }

  connection_t::connection_t(const connection_config_t& config)
      : _handle(mysql_init(nullptr), detail::connection_cleanup), _debug(config.debug)
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
}  // namespace sqlpp::mysql

