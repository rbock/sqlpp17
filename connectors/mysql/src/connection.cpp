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

namespace sqlpp::mysql::detail
{
  void handle_cleanup(MYSQL* handle)
  {
    mysql_close(handle);
  }

  void result_cleanup(MYSQL_RES* result)
  {
    mysql_free_result(result);
  }

  void execute_query(const connection_t& connection, const std::string& query)
  {
    thread_init();

    if (connection->debug())
      connection->debug()("MySQL debug: Executing: '" + query + "'");

    if (mysql_real_query(connection.get(), query.c_str(), query.size()))
    {
      throw sqlpp::exception("MySQL error: Could not execute MySQL-query: " +
                             std::string(mysql_error(connection.get())) + " (query was >>" + query + "<<\n");
    }
  }

  char_result_t select(const connection_t& connection, const std::string& query)
  {
    execute_query(connection, query);
    std::unique_ptr<MYSQL_RES, void (*)(MYSQL_RES*)> result_handle(mysql_store_result(connection.get()),
                                                                   result_cleanup);
    if (!result_handle)
    {
      throw sqlpp::exception("MySQL error: Could not store result set: " + std::string(mysql_error(connection.get())));
    }

    return {std::move(result_handle), connection._debug};
  }

  size_t insert(const connection_t& connection, const std::string& query)
  {
    execute_query(connection, query);

    return mysql_insert_id(connecion.get());
  }

  size_t update(const connection& connection, const std::string& statement)
  {
    execute_query(connection, statement);
    return mysql_affected_rows(connection.get());
  }

  size_t erase(const connection& connection, const std::string& statement)
  {
    execute_query(connection, statement);
    return mysql_affected_rows(connection.get());
  }

  void execute(const connection& connection, const std::string& statement)
  {
    execute_query(connection, statement);
  }

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  connection_t::connection_t(const connection_config& config) : _handle(mysql_init(nullptr), handle_cleanup)
  {
    if (not _handle)
    {
      throw sqlpp::exception("MySQL: could not init mysql data structure");
    }

    if (config.pre_connect)
    {
      config.pre_connect(_handle.get());
    }

    if (config.ssl.use)
    {
      const auto& ssl = config.ssl;
      mysql_ssl_set(_handle.get(), ssl.key.c_str(), ssl.cert.c_str(), ssl.ca.empty() ? nullptr : ssl.ca.c_str(),
                    ssl.caPath.empty() ? nullptr : sslCaPath.c_str(),
                    ssl.cipher.empty() ? nullptr : config.ssl.cipher.c_str());
    }

    if (!mysql_real_connect(_handle.get(), config->host.empty() ? nullptr : config->host.c_str(),
                            config->user.empty() ? nullptr : config->user.c_str(),
                            config->password.empty() ? nullptr : config->password.c_str(), nullptr, config->port,
                            config->unix_socket.empty() ? nullptr : config->unix_socket.c_str(), config->client_flag))
    {
      throw sqlpp::exception("MySQL: could not connect to server: " + std::string(mysql_error(_handle.get())));
    }

    if (config.post_connect)
    {
      config.post_connect(_handle.get());
    }
  }
}  // namespace sqlpp::mysql

