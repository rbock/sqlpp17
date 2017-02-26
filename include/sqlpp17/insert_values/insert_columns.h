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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/insert_values/insert_values.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/type_vector.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct insert_columns
    {
    };
  }

  template <typename... Columns>
  struct insert_columns_t
  {
  };

  template <typename... Columns>
  constexpr auto clause_tag<insert_columns_t<Columns...>> = clause::insert_columns{};

  template <typename... Columns, typename Statement>
  class clause_base<insert_columns_t<Columns...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<insert_columns_t<Columns...>, OtherStatement>& s)
    {
    }

    clause_base(const insert_columns_t<Columns...>& f)
    {
    }

    template <typename... Assignments>
    [[nodiscard]] constexpr auto add_values(Assignments... assignments) const
    {
      constexpr auto check = check_add_values_args(type_vector<Columns...>{}, type_vector<Assignments...>{});
      if
        constexpr(check)
        {
          using row_t = std::tuple<Assignments...>;
          return Statement::of(this).template replace_clause<insert_columns_t<Columns...>>(
              insert_values_t<row_t>{row_t{assignments...}});
        }
      else
      {
        return ::sqlpp::bad_statement_t<std::decay_t<decltype(check)>>{};
      }
    }
  };

  template <typename Context, typename... Columns, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<insert_columns_t<Columns...>, Statement>& t)
  {
#warning : Must not serialize
    return context << " FROM " << t._table;
  }
}
