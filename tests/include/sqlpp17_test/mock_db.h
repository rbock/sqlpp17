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

#include <sqlpp17/connection_base.h>
#include <sqlpp17/context_base.h>
#include <sqlpp17/result.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/transaction.h>

namespace sqlpp::test
{
  struct mock_context_t : public ::sqlpp::context_base
  {
  };

  struct mock_result
  {
    [[nodiscard]] operator bool() const
    {
      return false;  // no more rows available
    }
  };

  struct prepared_statement
  {
  };

  template <typename Parameter>
  auto bind_parameter(prepared_statement&, [[maybe_unused]] const Parameter& parameter, [[maybe_unused]] int index)
  {
  }

  struct mock_prepared_insert : public prepared_statement
  {
    [[nodiscard]] auto run()
    {
      return std::size_t{};
    }
  };

  struct mock_prepared_select : public prepared_statement
  {
    [[nodiscard]] auto run()
    {
      return mock_result{};
    }
  };

  template <typename Row>
  auto get_next_result_row(mock_result& result, Row& row)
  {
  }

  class mock_db : public ::sqlpp::connection_base
  {
    template <typename... Clauses>
    friend class ::sqlpp::statement;

    friend class ::sqlpp::transaction_t<mock_db>;

    template <typename Clause, typename Statement>
    friend class ::sqlpp::clause_base;

  public:
    template <typename... Clauses>
    auto operator()(const ::sqlpp::statement<Clauses...>& statement)
    {
      using Statement = ::sqlpp::statement<Clauses...>;
      if constexpr (constexpr auto _check = check_statement_executable<mock_db>(type_v<Statement>); _check)
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
      if constexpr (constexpr auto _check = check_statement_preparable<mock_db>(type_v<Statement>); _check)
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
    }

    auto commit() -> void
    {
    }

    auto rollback() -> void
    {
    }

    auto destroy_transaction() noexcept -> void
    {
    }

    template <typename Statement>
    auto execute(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return;
    }

    template <typename Statement>
    auto insert(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_insert(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return prepared_statement_t{statement, mock_prepared_insert{}};
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return 0ull;
    }

    template <typename Statement>
    auto delete_from(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return 0ull;
    }

    template <typename Statement>
    auto select(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return ::sqlpp::result_t<result_row_of_t<Statement>, mock_result>{mock_result{}};
    }

    template <typename Statement>
    [[nodiscard]] auto prepare_select(const Statement& statement)
    {
      [[maybe_unused]] auto x = to_sql_string_c(mock_context_t{}, statement);
      return prepared_statement_t{statement, mock_prepared_select{}};
    }
  };

}  // namespace sqlpp::test
