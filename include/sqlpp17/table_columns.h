#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
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

#include <sqlpp17/column.h>
#include <sqlpp17/member.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename TypeVector>
  struct table_columns
  {
    static_assert(wrong<table_columns>, "Invalid arguments for table_columns");
  };

  template <typename TableSpec, typename... ColumnSpecs>
  struct table_columns<TableSpec, type_vector<ColumnSpecs...>>
      : public member_t<ColumnSpecs, column_t<TableSpec, ColumnSpecs>>...
  {
  };

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] constexpr auto column_tuple_of(const table_columns<TableSpec, type_vector<ColumnSpecs...>>& t)
  {
    return std::tuple{static_cast<member_t<ColumnSpecs, column_t<TableSpec, ColumnSpecs>>>(t)()...};
  }

  template <typename TableSpec, typename... ColumnSpecs>
  [[nodiscard]] constexpr auto all_of(const table_columns<TableSpec, type_vector<ColumnSpecs...>>& t)
  {
    return std::tuple{column_t<TableSpec, ColumnSpecs>{}...};
  }

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto required_insert_columns_of_v<table_columns<TableSpec, type_vector<ColumnSpecs...>>> =
      type_set_if<is_insert_required, column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto default_columns_of_v<table_columns<TableSpec, type_vector<ColumnSpecs...>>> =
      type_set_if<has_default, column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto columns_of_v<table_columns<TableSpec, type_vector<ColumnSpecs...>>> =
      type_set<column_t<TableSpec, ColumnSpecs>...>();

  template <typename TableSpec, typename... ColumnSpecs>
  constexpr auto can_be_null_columns_of_v<table_columns<TableSpec, type_vector<ColumnSpecs...>>> =
      type_set_if<can_be_null, column_t<TableSpec, ColumnSpecs>...>();

}  // namespace sqlpp
