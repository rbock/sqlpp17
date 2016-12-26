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
#include <sqlpp17/all.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/statement_constructor_arg.h>
#include <sqlpp17/type_traits.h>

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
      if
        constexpr(is_result_clause_v<Rhs>)
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

  template <typename... Clauses>
  class statement : public clause_base<Clauses, statement<Clauses...>>...,
                    public result_base<get_result_clause_t<Clauses...>, statement<Clauses...>>
  {
    template <typename, typename>
    friend class clause_base;

    template <typename, typename>
    friend class result_base;

    using clauses = type_vector<Clauses...>;

    template <typename... Cs>
    using new_statement = statement<Cs...>;

    template <typename Base>
    static auto of(const Base* base)
    {
      return static_cast<const statement&>(*base);
    }

    [[nodiscard]] constexpr auto check_consistency() const
    {
      return (succeeded{} && ... && check_consistency(static_cast<const clause_base<Clauses, statement>&>(*this)));
    }

  public:
    constexpr statement()
    {
    }

    template <typename Arg>
    constexpr statement(Arg&& arg) : clause_base<Clauses, statement>(arg)...
    {
    }

    template <typename OldClause, typename NewClause>
    auto replace_clause(NewClause&& newClause) const
    {
      using new_clauses = algorithm::replace_t<clauses, OldClause, std::decay_t<NewClause>>;
      return algorithm::copy_t<new_clauses, new_statement>{
          detail::make_constructor_arg(*this, std::forward<NewClause>(newClause))};
    }
  };

  template <typename... Clauses>
  constexpr auto is_statement_v<statement<Clauses...>> = true;

  template <typename... Clauses>
  constexpr auto requires_braces_v<statement<Clauses...>> = true;

  SQLPP_WRAPPED_STATIC_ASSERT(assert_statement_contains_tagged_clauses, "statements must contain tagged clauses");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_statement_contains_unique_clauses,
                              "statements must contain uniquely tagged clauses (except custom clauses)");
#warning : Need to check statement clauses for execution of the statement

  template <typename... Clauses>
  constexpr auto check_statement_clauses()
  {
    constexpr auto count_custom_tags =
        (std::size_t{} + ... + std::is_same_v<decltype(clause_tag<Clauses>), clause::custom>);
    if
      constexpr(all<is_tagged_clause_v<Clauses>...>)
      {
        return failed<assert_statement_contains_tagged_clauses>{};
      }
    else if
      constexpr((type_set<Clauses...>() - type_set<clause::custom>()).size() != sizeof...(Clauses) - count_custom_tags)
      {
        return failed<assert_statement_contains_unique_clauses>{};
      }
    else
      return succeeded{};
  }

  template <typename Context, typename... Clauses>
  decltype(auto) operator<<(Context& context, const statement<Clauses...>& t)
  {
    return (context << ... << static_cast<const clause_base<Clauses, statement<Clauses...>>&>(t));
  }

  template <typename... LClauses, typename... RClauses>
  constexpr auto operator<<(statement<LClauses...> l, statement<RClauses...> r)
  {
    constexpr auto check = check_statement_clauses<LClauses..., RClauses...>();
    if
      constexpr(check)
      {
        return statement<LClauses..., RClauses...>(detail::make_constructor_arg(l, r));
      }
    else
    {
      return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
    }
  }
}
