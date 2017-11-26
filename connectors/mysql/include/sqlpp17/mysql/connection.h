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

#include <type_traits>
#include <sqlpp17/type_vector.h>
#include <sqlpp17/wrong.h>

namespace sqlpp::mysql::detail
{
  // direct execution
  char_result_t select(const connection_t&, const std::string& statement);
  size_t insert(const connection_t&, const std::string& statement);
  size_t update(const connection_t&, const std::string& statement);
  size_t erase(const connection_t&, const std::string& statement);
  void execute(const connection_t&, const std::string& statement);

  // prepared execution
  prepared_statement_t prepare(const connection_t&,
                               const std::string& statement,
                               size_t no_of_parameters,
                               size_t no_of_columns);
  bind_result_t run_prepared_select(const connection_t&, prepared_statement_t& prepared_statement);
  size_t run_prepared_insert(const connection_t&, prepared_statement_t& prepared_statement);
  size_t run_prepared_update(const connection_t&, prepared_statement_t& prepared_statement);
  size_t run_prepared_erase(const connection_t&, prepared_statement_t& prepared_statement);

}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  class connection
  {
    template <typename... Clauses>
    friend class ::sqlpp::statement;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::result_base;

  public:
    connection() = delete;
    connection(const connection_config& config);
    connection(const connection&) = delete;
    connection(connection&&) = default;
    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = default;
    ~connection() = default;

    template <typename Statement>
    auto operator()(const Statement& statement)
    {
#warning Need to do a final consistency check here
      return statement.run(*this);
    }

    auto execute(const std::string& query)
    {
      return detail::execute(*this, query);
    }

#warning : Need to implement
    template <typename Statement>
    auto prepare(const Statement& statement)
    {
    }

    MYSQL* get() const
    {
      return _handle.get();
    }

  private:
    template <typename Statement>
    auto insert(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_insert(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename PreparedStatement>
    [[nodiscard]] auto run_prepared_insert(const PreparedStatement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_update(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename PreparedStatement>
    [[nodiscard]] auto run_prepared_update(const PreparedStatement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    auto erase(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_erase(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename PreparedStatement>
    [[nodiscard]] auto run_prepared_erase(const PreparedStatement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
#warning implement
      return ::sqlpp::result_t<typename Statement::_result_row_t, mock_handle>{std::make_unique<mock_handle>()};
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_select(const Statement& statement)
    {
#warning implement
      return ::sqlpp::result_t<typename Statement::_result_row_t, mock_handle>{std::make_unique<mock_handle>()};
    }

    template <typename PreparedStatement>
    [[nodiscard]] auto run_prepared_select(const PreparedStatement& statement)
    {
#warning implement
      return ::sqlpp::result_t<typename PreparedStatement::_result_row_t, mock_handle>{std::make_unique<mock_handle>()};
    }

    std::unique_ptr<MYSQL, void (*)(MYSQL*)> _handle;
  };

}  // namespace sqlpp::mysql
