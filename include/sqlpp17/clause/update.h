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

#include <sqlpp17/clause/update_set.h>
#include <sqlpp17/clause/update_table.h>
#include <sqlpp17/clause/where.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  namespace clause
  {
    struct update
    {
    };
  }  // namespace clause

  struct update_t
  {
  };

  template <>
  constexpr auto clause_tag<update_t> = clause::update{};

  template <typename Statement>
  class clause_base<update_t, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<update_t, OtherStatement>&)
    {
    }

    clause_base() = default;
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<update_t, Statement>& t)
  {
    return std::string{"UPDATE"};
  }

  [[nodiscard]] constexpr auto update()
  {
    return statement<update_t, no_update_table_t, no_update_set_t, no_where_t>{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto update(Table&& t)
  {
    return update().table(std::forward<Table>(t));
  }
}  // namespace sqlpp
