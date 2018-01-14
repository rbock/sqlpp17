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

namespace sqlpp::postgresql
{
  class connection_t;
  class connection_pool_t;

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

  // direct execution
  auto select(const connection_t&, const std::string& statement) -> char_result_t;
  auto insert(const connection_t&, const std::string& statement) -> std::size_t;
  auto update(const connection_t&, const std::string& statement) -> std::size_t;
  auto delete_from(const connection_t&, const std::string& statement) -> std::size_t;
  auto execute(const connection_t&, const std::string& statement) -> void;

  // prepared execution
  auto prepare(const connection_t&, const std::string& statement, size_t no_of_parameters, size_t no_of_columns)
      -> ::sqlpp::postgresql::prepared_statement_t;

  class prepared_select_t : public ::sqlpp::postgresql::prepared_statement_t
  {
  public:
    prepared_select_t(::sqlpp::postgresql::prepared_statement_t&& statement)
        : ::sqlpp::postgresql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> char_result_t;
  };

  class prepared_insert_t : public ::sqlpp::postgresql::prepared_statement_t
  {
  public:
    prepared_insert_t(::sqlpp::postgresql::prepared_statement_t&& statement)
        : ::sqlpp::postgresql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_update_t : public ::sqlpp::postgresql::prepared_statement_t
  {
  public:
    prepared_update_t(::sqlpp::postgresql::prepared_statement_t&& statement)
        : ::sqlpp::postgresql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

  class prepared_delete_from_t : public ::sqlpp::postgresql::prepared_statement_t
  {
  public:
    prepared_delete_from_t(::sqlpp::postgresql::prepared_statement_t&& statement)
        : ::sqlpp::postgresql::prepared_statement_t(std::move(statement))
    {
    }

    auto run() -> std::size_t;
  };

}  // namespace sqlpp::postgresql::detail

namespace sqlpp::postgresql
{
  class connection_t : public ::sqlpp::connection_base
  {
    detail::unique_connection_ptr _handle;
    connection_pool_t* _connection_pool = nullptr;
    bool _transaction_active = false;
    std::function<void(std::string_view)> _debug;

    mutable std::size_t _statement_index = 0;

    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::result_base;

    friend class ::sqlpp::postgresql::connection_pool_t;

    connection_t(const connection_config_t& config,
                 detail::unique_connection_ptr&& handle,
                 connection_pool_t* connection_pool)
        : _handle(std::move(handle)), _connection_pool(connection_pool), _debug(config.debug)
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
        return ::sqlpp::bad_expression_t{check};
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
        return ::sqlpp::bad_expression_t{check};
      }
    }

    auto start_transaction() -> void;

    auto commit() -> void;

    auto rollback() -> void;

    auto destroy_transaction() noexcept -> void;

    auto debug() const
    {
      return _debug;
    }

    auto* get() const
    {
      return _handle.get();
    }

    auto is_alive() -> bool;

    auto get_statement_index() const
    {
      return ++_statement_index;
    }

  private:
    template <typename... Clauses>
    auto execute(const ::sqlpp::statement<Clauses...>& statement)
    {
      return detail::execute(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    auto insert(const Statement& statement)
    {
      return detail::insert(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_insert(const Statement& statement)
    {
      return detail::prepared_insert_t{
          detail::prepare(*this, to_sql_string_c(context_t{}, statement), statement.get_no_of_parameters(), 0)};
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
      return detail::update(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_update(const Statement& statement)
    {
      return detail::prepared_update_t{
          detail::prepare(*this, to_sql_string_c(context_t{}, statement), statement.get_no_of_parameters(), 0)};
    }

    template <typename Statement>
    auto delete_from(const Statement& statement)
    {
      return detail::delete_from(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_delete_from(const Statement& statement)
    {
      return detail::prepared_delete_from_t{
          detail::prepare(*this, to_sql_string_c(context_t{}, statement), statement.get_no_of_parameters(), 0)};
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
      return detail::select(*this, to_sql_string_c(context_t{}, statement));
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_select(const Statement& statement)
    {
      return detail::prepared_select_t{detail::prepare(*this, to_sql_string_c(context_t{}, statement),
                                                       statement.get_no_of_parameters(),
                                                       statement.get_no_of_result_columns())};
    }
  };

}  // namespace sqlpp::postgresql
