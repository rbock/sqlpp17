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

#include <tuple>
#include <vector>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/insert_column_printer.h>
#include <sqlpp17/detail/insert_value_printer.h>
#include <sqlpp17/exception.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct insert_multi_values
    {
    };
  }

  template <typename... Assignments>
  struct insert_multi_values_t
  {
    std::vector<std::tuple<Assignments...>> _rows;
  };

  template <typename... Assignments>
  constexpr auto clause_tag<insert_multi_values_t<Assignments...>> = clause::insert_multi_values{};

  template <typename Statement, typename... Assignments>
  class clause_base<insert_multi_values_t<Assignments...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<insert_multi_values_t<Assignments...>, OtherStatement>& s) : _rows(s._rows)
    {
    }

    clause_base(const insert_multi_values_t<Assignments...>& f) : _rows(f._rows)
    {
    }

    std::vector<std::tuple<Assignments...>> _rows;
  };

  namespace detail
  {
    template <typename Context>
    struct multi_row_insert_column_printer
    {
      Context& context;
      bool is_first = true;

      template <typename Expr>
      decltype(auto) operator()(const Expr& expr)
      {
        if (is_first)
          is_first = false;
        else
          context << ", ";
        if
          constexpr(is_optional(expr))
          {
            if (expr.to_be_used)
            {
              context << expr.value;
            }
            else
            {
              context << "default";
            }
          }
      }
    };
  }

  template <typename Context, typename Statement, typename... Assignments>
  decltype(auto) operator<<(Context& context, const clause_base<insert_multi_values_t<Assignments...>, Statement>& t)
  {
#warning : Might be nice to have an is_noop function that just does nothing and returns the default value of the return type
    if (t._rows.empty())
      throw ::sqlpp::exception("empty multi row insert");

    // columns
    {
      context << " (";
      auto print = detail::insert_column_printer(context);
      (..., print(std::get<Assignments>(t._assignments)));
      context << ")";
    }

    // values
    {
      context << " VALUES (";
      auto separate_rows = separator(context, ",");
      for (const auto& row : t._rows)
      {
        separate_rows();
        context << '(';
        auto print = detail::insert_value_printer(context);
        (..., print(std::get<Assignments>(row)));
        context << ')';
      }
    }

    return context;
  }
}
