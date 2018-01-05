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
#include <sqlpp17/multi_column.h>
#include <sqlpp17/table_alias.h>
#include <sqlpp17/to_sql_name.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename... ColumnSpecs>
  class table_t : public join_functions<table_t<TableSpec, ColumnSpecs...>>,
                  public member_t<ColumnSpecs, column_t<TableSpec, ColumnSpecs>>...
  {
  public:
    template <typename NamedTypeOrTag>
    [[nodiscard]] constexpr auto as([[maybe_unused]] const NamedTypeOrTag&) const
    {
      if constexpr (std::is_base_of_v<::sqlpp::name_tag_base, NamedTypeOrTag>)
      {
        return table_alias_t<table_t, table_spec<NamedTypeOrTag, type_hash<table_t>()>, ColumnSpecs...>{{}};
      }
      else if constexpr (not std::is_same_v<name_tag_of_t<NamedTypeOrTag>, none_t>)
      {
        return table_alias_t<table_t, table_spec<name_tag_of_t<NamedTypeOrTag>, type_hash<table_t>()>, ColumnSpecs...>{
            {}};
      }
      else
      {
        static_assert(wrong<NamedTypeOrTag>,
                      "as() is expecting a named expression (e.g. column, table), or a name tag");
      }
    }
  };

  template <typename TableSpec, typename... ColumnSpecs>
  struct name_tag_of<table_t<TableSpec, ColumnSpecs...>>
  {
    using type = typename TableSpec::_sqlpp_name_tag;
  };

  template <typename TableSpec, typename... ColumnSpecs>
  struct table_spec_of<table_t<TableSpec, ColumnSpecs...>>
  {
    using type = TableSpec;
  };

  template <typename Context, typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] auto to_sql_string(Context& context, const table_t<TableSpec, ColumnSpecs...>& t)
  {
    return to_sql_name(context, t);
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] constexpr auto column_tuple_of(const table_t<TableSpec, ColumnSpecs...>& t)
  {
    return std::tuple{static_cast<member_t<ColumnSpecs, column_t<TableSpec, ColumnSpecs>>>(t)()...};
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] constexpr auto all_of(const table_t<TableSpec, ColumnSpecs...>& t)
  {
    return multi_column_t{column_t<TableSpec, ColumnSpecs>{}...};
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] constexpr auto provided_tables_of([[maybe_unused]] type_t<table_t<TableSpec, ColumnSpecs...>>)
  {
    return type_set<TableSpec>();
  }

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto is_table_v<table_t<TableSpec, ColumnSpecs...>> = true;

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto table_names_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set<char_sequence_of_t<table_t<TableSpec, ColumnSpecs...>>>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto required_insert_columns_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set_if<is_insert_required, column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto default_columns_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set_if<has_default, column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto columns_of_v<table_t<TableSpec, ColumnSpecs...>> = type_set<column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto can_be_null_columns_of_v<table_t<TableSpec, ColumnSpecs...>> =
      type_set_if<can_be_null, column_t<TableSpec, ColumnSpecs>...>();

}  // namespace sqlpp
