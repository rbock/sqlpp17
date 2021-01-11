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

#include <sqlpp17/core/clause_fwd.h>
#include <sqlpp17/core/remove.h>
#include <sqlpp17/core/type_traits.h>
#include <sqlpp17/core/where.h>

#include <sqlpp17/postgresql/remove_table.h>
#include <sqlpp17/postgresql/remove_using.h>

namespace sqlpp::postgresql
{
  [[nodiscard]] constexpr auto remove()
  {
    return statement<remove_t, postgresql::no_remove_table_t, postgresql::no_remove_using_t, no_where_t>{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto remove_from(Table&&... table)
  {
#warning : see https://www.postgresql.org/docs/9.1/static/sql-delete.html
    /*
     * ONLY keyword
     * using not required
     * using may contain a join that must not contain the table from remove_table (it may contain an alias, though)
     */
    return remove().from(std::forward<Tables>(tables)...);
  }
}  // namespace sqlpp::postgresql
