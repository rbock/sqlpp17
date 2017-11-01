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
#include <sqlpp17/join.h>
#include <sqlpp17/member.h>
#include <sqlpp17/table_alias.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename... ColumnSpecs>
  class table_t : public join_functions<table_t<TableSpec, ColumnSpecs...>>,
                  public member_t<ColumnSpecs, column_t<TableSpec, ColumnSpecs>>...
  {
  public:
    using _alias_t = typename TableSpec::_alias_t;

    template <typename Alias>
    constexpr auto as(const Alias&) const
    {
      return table_alias_t<table_t, Alias, ColumnSpecs...>{{}};
    }
  };

  template <typename Context, typename TableSpec, typename... ColumnSpecs>
  decltype(auto) operator<<(Context& context, const table_t<TableSpec, ColumnSpecs...>& t)
  {
    return context << (name_of_v<TableSpec>);
  }

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto is_table_v<table_t<TableSpec, ColumnSpecs...>> = true;

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto char_sequence_of_v<table_t<TableSpec, ColumnSpecs...>> = make_char_sequence_t<name_of_v<TableSpec>>{};

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto table_names_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set(char_sequence_of_v<table_t<TableSpec, ColumnSpecs...>>);

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto required_insert_columns_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set_if<is_insert_required, column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto columns_of_v<table_t<TableSpec, ColumnSpecs...>> = type_set<column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto optional_columns_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set_if<is_value_type_optional, column_t<TableSpec, ColumnSpecs>...>();

}  // namespace sqlpp
