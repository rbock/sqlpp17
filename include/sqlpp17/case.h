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

#include <sqlpp17/embrace.h>
#include <sqlpp17/tuple_to_sql_string.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename CaseWhenThen, typename Else>
  struct case_when_then_else_t
  {
    CaseWhenThen _case_when_then;  // one or more when/then combinations
    Else _else;                    // The final else
  };

  template <typename CaseWhenThen, typename Else>
  struct nodes_of<case_when_then_else_t<CaseWhenThen, Else>>
  {
    using type = type_vector<CaseWhenThen, Else>;
  };

  template <typename Expr>
  struct then_t
  {
    Expr _expr;
  };

  template <typename When, typename Then>
  struct when_then_t
  {
    When _when;
    Then _then;
  };

  template <typename When, typename Then>
  struct nodes_of<when_then_t<When, Then>>
  {
    using type = type_vector<When, Then>;
  };

  template <typename... WhenThen>
  struct case_when_then_t
  {
    std::tuple<WhenThen...> _when_thens;

    template <typename When, typename Then>
    [[nodiscard]] constexpr auto when(When when, then_t<Then> then) const
    {
#warning : Checks missing
      auto _wt = when_then_t<When, Then>{when, then._expr};
      return case_when_then_t<WhenThen..., decltype(_wt)>{std::tuple_cat(_when_thens, std::tuple{_wt})};
    }

    template <typename Expr>
    [[nodiscard]] constexpr auto else_(Expr expr) const
    {
#warning : Checks missing
      return case_when_then_else_t<case_when_then_t, Expr>{*this, expr};
    }
  };

  template <typename... WhenThen>
  struct nodes_of<case_when_then_t<WhenThen...>>
  {
    using type = type_vector<WhenThen...>;
  };

  template <typename DbConnection, typename When, typename Then>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const when_then_t<When, Then>& t)
  {
    return std::string{" WHEN "} + to_sql_string(connection, embrace(t._when)) + " THEN " +
           to_sql_string(connection, embrace(t._then));
  }

  template <typename DbConnection, typename... WhenThen>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const case_when_then_t<WhenThen...>& t)
  {
    return std::string{" CASE"} + tuple_to_sql_string(connection, "", t._when_thens);
  }

  template <typename DbConnection, typename CaseWhenThen, typename Else>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const case_when_then_else_t<CaseWhenThen, Else>& t)
  {
    return to_sql_string(connection, t._case_when_then) + " ELSE " + to_sql_string(connection, embrace(t._else));
  }

  template <typename Expr>
  [[nodiscard]] constexpr auto then(Expr expr)
  {
    return then_t<Expr>{expr};
  }

  template <typename When, typename Then>
  [[nodiscard]] constexpr auto case_when(When when, then_t<Then> then)
  {
#warning : Checks missing
    auto wt = when_then_t<When, Then>{when, then._expr};
    return case_when_then_t<decltype(wt)>{std::tuple{wt}};
  }

}  // namespace sqlpp
