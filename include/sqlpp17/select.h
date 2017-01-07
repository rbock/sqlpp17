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
#include <sqlpp17/from.h>
#include <sqlpp17/having.h>
#include <sqlpp17/selected_columns.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/where.h>

namespace sqlpp
{
  namespace clause
  {
    struct select
    {
    };
  }

  struct select_t
  {
  };

  template <>
  constexpr auto clause_tag<select_t> = clause::select{};

  template <typename Statement>
  class clause_base<select_t, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<select_t, OtherStatement>&)
    {
    }

    clause_base() = default;
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<select_t, Statement>& t)
  {
    return context << "SELECT";
  }

  [[nodiscard]] constexpr auto select()
  {
    return statement<select_t, no_selected_columns_t, no_from_t, no_where_t, no_having_t>{};
  }

  template <typename... Fs>
  [[nodiscard]] constexpr auto select(Fs... fs)
  {
    /*
    else if
      constexpr((true && ... is_flag_v<Fs>))
      {
      return ::sqlpp::select().flags(fs...);
      }
      */
    if
      constexpr((true && ... && is_selectable_v<Fs>))
      {
        return ::sqlpp::select().columns(fs...);
      }
    else
    {
#warning Need to add handling for flags and errors here
      return 17;  //::sqlpp::bad_statement_t<assert_select_args_are_flags_or_columns>{};
    }
  }
}
