#pragma once

/*
Copyright (c) 2016, Roland Bock
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

#include <sqlpp17/algorithm.h>
#include <sqlpp17/bad_statement.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/statement_constructor_arg.h>
#include <sqlpp17/prepared_statement.h>
#include <sqlpp17/succeeded.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  struct no_result
  {
  };

  template <typename Statement>
  class result_base<no_result, Statement>
  {
  };

  template <>
  constexpr auto is_result_clause_v<no_result> = true;  // yes, no result is also a result :-)

  template <typename T>
  struct result_wrapper
  {
    using type = T;

    template <typename Rhs>
    constexpr auto operator<<(const result_wrapper<Rhs>&)
    {
      if constexpr (is_result_clause_v<Rhs>)
      {
        return result_wrapper<Rhs>{};
      }
      else
      {
        return result_wrapper<T>{};
      }
    }
  };

  template <typename... Clauses>
  struct get_result_clause
  {
    using type = typename decltype((result_wrapper<no_result>{} << ... << result_wrapper<Clauses>{}))::type;
  };

  template <typename... Clauses>
  using get_result_clause_t = typename get_result_clause<Clauses...>::type;

  template <typename Db, typename Clause, typename Statement>
  constexpr auto check_clause_preparable(const type_t<clause_base<Clause, Statement>>&)
  {
    return succeeded{};
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_all_required_tables_are_provided, "statement uses tables that are not provided");

  template <typename Db, typename... Clauses>
  constexpr auto check_statement_preparable([[maybe_unused]] const type_t<statement<Clauses...>>& s)
  {
    using _statement_t = statement<Clauses...>;

    if constexpr (not(required_tables_of_v<_statement_t> <= provided_tables_of_v<_statement_t>))
    {
      return failed<assert_all_required_tables_are_provided>{};
    }
    else
      return (succeeded{} and ... and check_clause_preparable<Db>(type_v<clause_base<Clauses, _statement_t>>));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_execute_without_parameters,
                              "directly executed statements must have no parameters");

  template <typename Db, typename... Clauses>
  constexpr auto check_statement_executable(const type_t<statement<Clauses...>>& s)
  {
    using _statement_t = statement<Clauses...>;
    if constexpr (_statement_t::get_no_of_parameters() != 0)
    {
      return failed<assert_execute_without_parameters>{};
    }
    else
      return check_statement_preparable<Db>(s);
  }

  template <typename... Clauses>
  class statement : public clause_base<Clauses, statement<Clauses...>>...,
                    public result_base<get_result_clause_t<Clauses...>, statement<Clauses...>>
  {
    template <typename, typename>
    friend class clause_base;

    template <typename, typename>
    friend class result_base;

    using _clauses = type_vector<Clauses...>;

    template <typename... Cs>
    using new_statement = statement<Cs...>;

    template <typename Base>
    static auto of(const Base* base)
    {
      return static_cast<const statement&>(*base);
    }

    template <typename OldClause, typename NewClause>
    static auto replace_clause(const clause_base<OldClause, statement>* base, NewClause newClause)
    {
      const auto& old_statement = statement::of(base);
      using new_clauses = algorithm::replace_t<_clauses, OldClause, NewClause>;
      return algorithm::copy_t<new_clauses, new_statement>{detail::statement_constructor_arg(old_statement, newClause)};
    }

  public:
    constexpr statement()
    {
    }

    template <typename Arg>
    constexpr statement(Arg arg) : clause_base<Clauses, statement>(arg)...
    {
    }

    using result_base_t = result_base<get_result_clause_t<Clauses...>, statement<Clauses...>>;
    using result_row_t = result_row_of_t<result_base_t>;

    [[nodiscard]] static constexpr auto get_no_of_parameters()
    {
#warning : implement
      return 0;
    }

    template <typename Connection>
    [[nodiscard]] auto run(Connection& connection) const
    {
      if constexpr (constexpr auto check = check_statement_executable<Connection>(type_v<statement>); check)
      {
        return result_base_t::_run(connection);
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename Connection>
    [[nodiscard]] auto prepare(Connection& connection) const
    {
      if constexpr (constexpr auto check = check_statement_preparable<Connection>(type_v<statement>); check)
      {
        return result_base_t::_prepare(connection);
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename... Clauses>
  struct nodes_of<statement<Clauses...>>
  {
    using type = type_vector<Clauses...>;
  };

  template <typename... Clauses>
  constexpr auto is_statement_v<statement<Clauses...>> = true;

  template <typename... Clauses>
  constexpr auto requires_braces_v<statement<Clauses...>> = true;

  template <typename... Clauses>
  struct result_row_of<statement<Clauses...>>
  {
    using type = typename statement<Clauses...>::result_row_t;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_statement_contains_unique_clauses,
                              "statements must contain uniquely tagged clauses only (except custom clauses)");

  template <typename... Clauses>
  constexpr auto check_statement_clauses()
  {
    constexpr auto count_untagged_clauses =
        (std::size_t{} + ... + std::is_same_v<std::decay_t<decltype(clause_tag<Clauses>)>, no_clause>);
    if constexpr ((type_set(clause_tag<Clauses>...) - type_set<no_clause>()).size() !=
                  sizeof...(Clauses) - count_untagged_clauses)
    {
      return failed<assert_statement_contains_unique_clauses>{};
    }
    else
      return succeeded{};
  }

  template <typename DbConnection, typename... Clauses>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const statement<Clauses...>& t)
  {
    return (std::string{} + ... +
            to_sql_string(connection, static_cast<const clause_base<Clauses, statement<Clauses...>>&>(t)));
  }

  template <typename... LClauses, typename... RClauses>
  constexpr auto operator<<(statement<LClauses...> l, statement<RClauses...> r)
  {
    constexpr auto check = check_statement_clauses<LClauses..., RClauses...>();
    if constexpr (check)
    {
      // remove non-clauses from left part
      using clauses_t = decltype(
          (type_vector<>{} + ... + std::conditional_t<is_clause_v<LClauses>, type_vector<LClauses>, type_vector<>>{}) +
          type_vector<RClauses...>{});
      return algorithm::copy_t<clauses_t, statement>(detail::statement_constructor_arg(l, r));
    }
    else
    {
      return ::sqlpp::bad_statement_t{check};
    }
  }
}  // namespace sqlpp
