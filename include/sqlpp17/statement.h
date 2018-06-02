#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
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
#include <sqlpp17/bad_expression.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/statement_constructor_arg.h>
#include <sqlpp17/prepared_statement.h>
#include <sqlpp17/succeeded.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
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
  struct result_clause_of<statement<Clauses...>>
  {
    using type = typename decltype((result_wrapper<no_result>{} << ... << result_wrapper<Clauses>{}))::type;
  };

  template <typename... Clauses>
  struct result_type_of<statement<Clauses...>>
  {
    using type = clause_result_type_t<result_clause_of_t<statement<Clauses...>>>;
  };

  template <typename T>
  using result_type_of_t = typename result_type_of<T>::type;

  template <typename Db, typename Clause, typename Statement>
  constexpr auto check_clause_preparable(const type_t<clause_base<Clause, Statement>>&)
  {
    return succeeded{};
  }

  namespace detail
  {
    template <typename... Ts>
    [[nodiscard]] constexpr auto have_unique_names(type_vector<Ts...>)
    {
      return type_set(char_sequence_of_v<Ts>...).size() == type_vector<Ts...>::size();
    }
  }  // namespace detail

  SQLPP_WRAPPED_STATIC_ASSERT(assert_statement_all_required_tables_are_provided,
                              "statement uses tables that are not provided");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_statement_parameters_have_unique_names, "statement parameters must be unique");

  template <typename Db, typename... Clauses>
  constexpr auto check_statement_preparable([[maybe_unused]] const type_t<statement<Clauses...>>& s)
  {
    using _statement_t = statement<Clauses...>;

    if constexpr (not detail::have_unique_names(parameters_of_t<_statement_t>{}))
    {
      return failed<assert_statement_parameters_have_unique_names>{};
    }
    else if constexpr (not(required_tables_of_v<_statement_t> <= provided_tables_of_v<_statement_t>))
    {
      return failed<assert_statement_all_required_tables_are_provided>{};
    }
    else
      return (succeeded{} and ... and check_clause_preparable<Db>(type_v<clause_base<Clauses, _statement_t>>));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_execute_without_parameters,
                              "directly executed statements must have no parameters");

  template <typename Db, typename... Clauses>
  constexpr auto check_statement_executable(const type_t<statement<Clauses...>>& s)
  {
    if constexpr (parameters_of_t<statement<Clauses...>>::size() != 0)
    {
      return failed<assert_execute_without_parameters>{};
    }
    else
      return check_statement_preparable<Db>(s);
  }

  template <typename... Clauses>
  class statement : public clause_base<Clauses, statement<Clauses...>>...
  {
  public:
    constexpr statement()
    {
    }

    template <typename Arg>
    constexpr statement(Arg arg) : clause_base<Clauses, statement>(arg)...
    {
    }
  };

  template <typename Clause, typename... Clauses>
  auto clause_of(const statement<Clauses...>& s)
  {
    return static_cast<const clause_base<Clause, statement<Clauses...>>&>(s);
  }

  template <typename Clause, typename... Clauses>
  auto statement_of(const clause_base<Clause, statement<Clauses...>>& base)
  {
    return static_cast<const statement<Clauses...>&>(base);
  }

  template <typename OldClause, typename... Clauses, typename NewClause>
  auto new_statement(const clause_base<OldClause, statement<Clauses...>>& oldBase, NewClause newClause)
  {
    const auto& old_statement = statement_of(oldBase);
    return statement<std::conditional_t<std::is_same_v<Clauses, OldClause>, NewClause, Clauses>...>{
        detail::statement_constructor_arg(old_statement, newClause)};
  }

  template <typename... Clauses>
  struct nodes_of<statement<Clauses...>>
  {
    using type = type_vector<Clauses...>;
  };

  template <typename... Clauses>
  struct is_statement<statement<Clauses...>> : public std::true_type
  {
  };

  template <typename... Clauses>
  constexpr auto requires_braces_v<statement<Clauses...>> = true;

  template <typename... Clauses>
  struct result_row_of<statement<Clauses...>>
  {
    using type = result_row_of_t<clause_base<result_clause_of_t<statement<Clauses...>>, statement<Clauses...>>>;
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

  template <typename Context, typename... Clauses>
  [[nodiscard]] auto to_sql_string(Context& context, const statement<Clauses...>& t)
  {
    return (std::string{} + ... +
            to_sql_string(context, static_cast<const clause_base<Clauses, statement<Clauses...>>&>(t)));
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
      return ::sqlpp::bad_expression_t{check};
    }
  }
}  // namespace sqlpp
