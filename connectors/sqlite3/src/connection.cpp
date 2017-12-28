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

#include <sqlpp17/sqlite3/connection.h>
#include <sqlpp17/sqlite3/connection_pool.h>

namespace sqlpp::sqlite3::detail
{
  namespace
  {
    auto prepare_impl(const connection_t& connection, const std::string& statement) -> unique_prepared_statement_ptr
    {
      if (connection.debug())
        connection.debug()("Preparing: '" + statement + "'");

      ::sqlite3_stmt* statement_ptr = nullptr;

      const auto rc = sqlite3_prepare_v2(connection.get(), statement.c_str(), static_cast<int>(statement.size()),
                                         &statement_ptr, nullptr);

      auto statement_handle = unique_prepared_statement_ptr(statement_ptr, prepared_statement_cleanup{});

      if (rc != SQLITE_OK)
      {
        throw sqlpp::exception(
            "Sqlite3 error: Could not prepare statement: " + std::string(sqlite3_errmsg(connection.get())) +
            " (statement was >>" + statement + "<<)\n");
      }

      return statement_handle;
    }
  }  // namespace

#warning : Need to have better names for the two versions of execute_prepared_statement
  auto execute_prepared_statement(::sqlite3_stmt* statement,
                                  ::sqlite3* connection,
                                  const std::function<void(std::string_view)>& debug) -> void
  {
    if (debug)
      debug("Executing prepared statement");

    switch (const auto rc = sqlite3_step(statement); rc)
    {
      case SQLITE_OK:
        [[fallthrough]];
      case SQLITE_ROW:
        [[fallthrough]];  // might occur if execute is called with a select
      case SQLITE_DONE:
        return;
      default:
        throw sqlpp::exception("Sqlite3 error: Could not execute statement: " + std::string(sqlite3_errstr(rc)));
    }
  }

  auto execute_prepared_statement(const prepared_statement_t& prepared_statement) -> void
  {
    sqlite3_reset(prepared_statement.get());

    execute_prepared_statement(prepared_statement.get(), prepared_statement.connection(), prepared_statement.debug());
  }

  auto execute_query(const connection_t& connection, const std::string& statement) -> void
  {
    auto prepared_statement = prepare_impl(connection, statement);
    execute_prepared_statement(prepared_statement.get(), connection.get(), connection.debug());
  }

  auto select(const connection_t& connection, const std::string& statement) -> prepared_statement_result_t
  {
    return {prepare_impl(connection, statement), connection.debug()};
  }

  auto insert(const connection_t& connection, const std::string& statement) -> size_t
  {
    execute_query(connection, statement);
    return sqlite3_last_insert_rowid(connection.get());
  }

  auto update(const connection_t& connection, const std::string& statement) -> size_t
  {
    execute_query(connection, statement);
    return sqlite3_changes(connection.get());
  }

  auto erase(const connection_t& connection, const std::string& statement) -> size_t
  {
    execute_query(connection, statement);
    return sqlite3_changes(connection.get());
  }

  auto execute(const connection_t& connection, const std::string& statement) -> void
  {
    execute_query(connection, statement);
  }

  auto prepare(const connection_t& connection, const std::string& statement) -> ::sqlpp::sqlite3::prepared_statement_t
  {
    return {prepare_impl(connection, statement), connection.get(), connection.debug()};
  }

  auto prepared_select_t::run() -> prepared_statement_result_t
  {
    sqlite3_reset(_statement.get());
    return {_statement};
  }

  auto prepared_insert_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return sqlite3_last_insert_rowid(_statement.connection());
  }

  auto prepared_update_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return sqlite3_changes(_statement.connection());
  }

  auto prepared_erase_t::run() -> size_t
  {
    execute_prepared_statement(_statement);
    return sqlite3_changes(_statement.connection());
  }

}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  connection_t::connection_t(const connection_config_t& config)
      : _handle(nullptr, detail::connection_cleanup{}), _debug(config.debug)
  {
    ::sqlite3* connection_ptr = nullptr;
    const auto rc = sqlite3_open_v2(config.path_to_database.c_str(), &connection_ptr, config.flags,
                                    config.vfs.empty() ? nullptr : config.vfs.c_str());
    _handle.reset(connection_ptr);

    if (rc != SQLITE_OK)
    {
      throw sqlpp::exception("Sqlite3 error: Can't open database: " + std::string(sqlite3_errmsg(connection_ptr)));
    }

#ifdef SQLITE_HAS_CODEC
    if (config.password.size() > 0)
    {
      const auto ret = sqlite3_key(sqlite, config.password.data(), config.password.size());
      if (ret != SQLITE_OK)
      {
        const std::string msg = sqlite3_errmsg(sqlite);
        throw sqlpp::exception("Sqlite3 error: Can't set password for database: " +
                               std::string(sqlite3_errmsg(connection_ptr)));
      }
    }
#endif
  }

#warning : There are a bunch of additional functions in the sqlpp11 connector

  connection_t::~connection_t()
  {
    if (_connection_pool)
    {
      _connection_pool->put(std::move(_handle));
    }
  }
}  // namespace sqlpp::sqlite3

