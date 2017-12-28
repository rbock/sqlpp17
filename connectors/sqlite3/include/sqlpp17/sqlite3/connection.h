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

#include <functional>
#include <type_traits>

#include <sqlpp17/result.h>
#include <sqlpp17/statement.h>

#include <sqlpp17/sqlite3/clauses.h>
#include <sqlpp17/sqlite3/connection_config.h>
#include <sqlpp17/sqlite3/direct_execution_result.h>
#include <sqlpp17/sqlite3/prepared_statement.h>
#include <sqlpp17/sqlite3/prepared_statement_result.h>

namespace sqlpp::sqlite3
{
  class connection_t;
  class connection_pool_t;

};  // namespace sqlpp::sqlite3

namespace sqlpp::sqlite3::detail
{
  struct connection_cleanup
  {
  public:
    auto operator()(::sqlite3* handle) -> void
    {
      auto rc = sqlite3_close(handle);
      if (rc != SQLITE_OK)
      {
        throw sqlpp::exception(std::string("Sqlite3 error: Can't close database: ") + sqlite3_errmsg(handle));
      }
    }
  };
  using unique_connection_ptr = std::unique_ptr<::sqlite3, detail::connection_cleanup>;

  // direct execution
  auto select(const connection_t&, const std::string& statement) -> direct_execution_result_t;
  auto insert(const connection_t&, const std::string& statement) -> std::size_t;
  auto update(const connection_t&, const std::string& statement) -> std::size_t;
  auto erase(const connection_t&, const std::string& statement) -> std::size_t;
  auto execute(const connection_t&, const std::string& statement) -> void;

  // prepared execution
  auto prepare(const connection_t&, const std::string& statement) -> ::sqlpp::sqlite3 ::prepared_statement_t;

  class prepared_select_t
  {
    ::sqlpp::sqlite3 ::prepared_statement_t _statement;
    std::function<void(std::string_view)> _debug;

  public:
    prepared_select_t(::sqlpp::sqlite3 ::prepared_statement_t&& statement, std::function<void(std::string_view)> debug)
        : _statement(std::move(statement)), _debug(debug)
    {
    }

    auto run() -> prepared_statement_result_t;
  };

  class prepared_insert_t
  {
    ::sqlpp::sqlite3 ::prepared_statement_t _statement;

  public:
    prepared_insert_t(::sqlpp::sqlite3 ::prepared_statement_t&& statement) : _statement(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_update_t
  {
    ::sqlpp::sqlite3 ::prepared_statement_t _statement;

  public:
    prepared_update_t(::sqlpp::sqlite3 ::prepared_statement_t&& statement) : _statement(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_erase_t
  {
    ::sqlpp::sqlite3 ::prepared_statement_t _statement;

  public:
    prepared_erase_t(::sqlpp::sqlite3 ::prepared_statement_t&& statement) : _statement(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  class connection_t
  {
    std::function<void(std::string_view)> _debug;
    detail::unique_connection_ptr _handle;
    connection_pool_t* _connection_pool = nullptr;

    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::result_base;

    friend class ::sqlpp::sqlite3 ::connection_pool_t;

    connection_t(const connection_config_t& config,
                 detail::unique_connection_ptr&& handle,
                 connection_pool_t* connection_pool)
        : _debug(config.debug), _handle(std::move(handle)), _connection_pool(connection_pool)
    {
    }

    connection_t(const connection_config_t& config, connection_pool_t* connection_pool) : connection_t(config)
    {
      _connection_pool = connection_pool;
    }

  public:
    connection_t() = delete;
    connection_t(const connection_config_t& config);
    connection_t(const connection_t&) = delete;
    connection_t(connection_t&&) = default;
    connection_t& operator=(const connection_t&) = delete;
    connection_t& operator=(connection_t&&) = default;
    ~connection_t();

    template <typename... Clauses>
    auto operator()(const ::sqlpp::statement<Clauses...>& statement)
    {
      if constexpr (constexpr auto check =
                        check_statement_executable<connection_t>(type_v<::sqlpp::statement<Clauses...>>);
                    check)
      {
        return statement.run(*this);
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    auto execute(const std::string& query)
    {
      return detail::execute(*this, query);
    }

    template <typename Statement>
    auto prepare(const Statement& statement)
    {
      if constexpr (constexpr auto check = check_statement_preparable<connection_t>(type_v<Statement>); check)
      {
        return statement.prepare(*this);
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    auto debug() const
    {
      return _debug;
    }

    auto* get() const
    {
      return _handle.get();
    }

    auto is_alive() -> bool;

  private:
    template <typename Statement>
    auto _execute(const Statement& statement)
    {
      return detail::execute(*this, to_sql_string(*this, statement));
    }

    template <typename Statement>
    auto insert(const Statement& statement)
    {
      return detail::insert(*this, to_sql_string(*this, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_insert(const Statement& statement)
    {
      return detail::prepared_insert_t{detail::prepare(*this, to_sql_string(*this, statement))};
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
      return detail::update(*this, to_sql_string(*this, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_update(const Statement& statement)
    {
      return detail::prepared_update_t{detail::prepare(*this, to_sql_string(*this, statement))};
    }

    template <typename Statement>
    auto erase(const Statement& statement)
    {
      return detail::erase(*this, to_sql_string(*this, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_erase(const Statement& statement)
    {
      return detail::prepared_erase_t{detail::prepare(*this, to_sql_string(*this, statement))};
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
      return detail::select(*this, to_sql_string(*this, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_select(const Statement& statement)
    {
      return detail::prepared_select_t{detail::prepare(*this, to_sql_string(*this, statement)), _debug};
    }
  };  // namespace sqlpp::sqlite3

}  // namespace sqlpp::sqlite3
