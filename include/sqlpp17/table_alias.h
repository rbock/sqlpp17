#pragma once

/*
Copyright (c) 2016 - 2017, Roland Bock
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

#include <sqlpp17/char_sequence.h>
#include <sqlpp17/column.h>
#include <sqlpp17/join.h>
#include <sqlpp17/member.h>
#include <sqlpp17/to_sql_name.h>

namespace sqlpp
{
  template <typename Table, typename NameTag, typename... ColumnSpecs>
  struct table_alias_t : public join_functions<table_alias_t<Table, NameTag, ColumnSpecs...>>,
                         public member_t<ColumnSpecs, column_t<NameTag, ColumnSpecs>>...
  {
    Table _table;
  };

  template <typename Table, typename NameTag, typename... ColumnSpecs>
  struct name_tag_of<table_alias_t<Table, NameTag, ColumnSpecs...>>
  {
    using type = NameTag;
  };

  template <typename Table, typename NameTag, typename... ColumnSpecs>
  constexpr auto is_table_v<table_alias_t<Table, NameTag, ColumnSpecs...>> = true;

  template <typename Table, typename NameTag, typename... ColumnSpecs>
  constexpr auto columns_of_v<table_alias_t<Table, NameTag, ColumnSpecs...>> =
      type_set<column_t<NameTag, ColumnSpecs>...>();

  template <typename DbConnection, typename Table, typename NameTag, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection,
                                   const table_alias_t<Table, NameTag, ColumnSpecs...>& t)
  {
    auto ret = std::string{};

    if constexpr (requires_braces<Table>)
      ret += "(";
    ret += to_sql_string(t._table);
    if constexpr (requires_braces<Table>)
      ret += ")";
    ret += " AS " + to_sql_name(t);

    return ret;
  }

}  // namespace sqlpp
