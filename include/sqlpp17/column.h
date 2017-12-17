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

#include <sqlpp17/alias.h>
#include <sqlpp17/expr.h>
#include <sqlpp17/name_to_sql_string.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableAlias, typename ColumnSpec>
  class column_t
  {
  public:
    using _alias_t = typename ColumnSpec::_alias_t;

    template <typename T>
    [[nodiscard]] auto operator=(T t) const
    {
      return assign(*this, t);
    }

    template <typename Pattern>
    [[nodiscard]] constexpr auto like(Pattern pattern) const
    {
      using ::sqlpp::like;
      return like(*this, pattern);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto in(Exprs... exprs) const
    {
      using ::sqlpp::in;
      return in(*this, exprs...);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto not_in(Exprs... exprs) const
    {
      using ::sqlpp::not_in;
      return not_in(*this, exprs...);
    }

    [[nodiscard]] constexpr auto is_null() const
    {
      using ::sqlpp::is_null;
      return is_null(*this);
    }

    [[nodiscard]] constexpr auto is_not_null() const
    {
      using ::sqlpp::is_not_null;
      return is_not_null(*this);
    }

    [[nodiscard]] constexpr auto asc() const
    {
      using ::sqlpp::asc;
      return asc(*this);
    }

    [[nodiscard]] constexpr auto desc() const
    {
      using ::sqlpp::desc;
      return desc(*this);
    }

    template <typename Alias>
    [[nodiscard]] constexpr auto as(const Alias& alias) const
    {
      using ::sqlpp::as;
      return as(*this, alias);
    }
  };  // namespace sqlpp

  template <typename TableSpec, typename ColumnSpec>
  struct value_type_of<column_t<TableSpec, ColumnSpec>>
  {
    using type = typename ColumnSpec::value_type;
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto char_sequence_of_v<column_t<TableSpec, ColumnSpec>> = make_char_sequence_t<name_of_v<ColumnSpec>>{};

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_expression_v<column_t<TableSpec, ColumnSpec>> = true;

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_selectable_v<column_t<TableSpec, ColumnSpec>> = true;

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto must_not_insert_v<column_t<TableSpec, ColumnSpec>> = !!(ColumnSpec::tags & tag::must_not_insert);

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto must_not_update_v<column_t<TableSpec, ColumnSpec>> = !!(ColumnSpec::tags & tag::must_not_update);

  template <typename TableSpec, typename ColumnSpec>
  struct has_default<column_t<TableSpec, ColumnSpec>>
  {
    static constexpr auto value = !!(ColumnSpec::tags & (tag::has_default | tag::can_be_null));
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_insert_required_v<column_t<TableSpec, ColumnSpec>> =
      !(ColumnSpec::tags & tag::has_default) && !(ColumnSpec::tags & tag::must_not_insert);

  template <typename DbConnection, typename TableSpec, typename ColumnSpec>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const column_t<TableSpec, ColumnSpec>& t)
  {
    return name_to_sql_string(connection, name_of_v<TableSpec>) + "." +
           name_to_sql_string(connection, name_of_v<ColumnSpec>);
  }

}  // namespace sqlpp
