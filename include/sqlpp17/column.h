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
#include <sqlpp17/operator.h>
#include <sqlpp17/to_sql_name.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename ColumnSpec>
  class column_t
  {
  public:
    template <typename T>
    [[nodiscard]] auto operator=(T t) const
    {
      return assign(*this, t);
    }

    template <typename Pattern>
    [[nodiscard]] constexpr auto like(Pattern pattern) const
    {
      return ::sqlpp::like(*this, pattern);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto in(Exprs... exprs) const
    {
      return ::sqlpp::in(*this, exprs...);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto not_in(Exprs... exprs) const
    {
      return ::sqlpp::not_in(*this, exprs...);
    }

    [[nodiscard]] constexpr auto is_null() const
    {
      return ::sqlpp::is_null(*this);
    }

    [[nodiscard]] constexpr auto is_not_null() const
    {
      return ::sqlpp::is_not_null(*this);
    }

    [[nodiscard]] constexpr auto asc() const
    {
      return ::sqlpp::asc(*this);
    }

    [[nodiscard]] constexpr auto desc() const
    {
      return ::sqlpp::desc(*this);
    }

    template <typename Alias>
    [[nodiscard]] constexpr auto as(const Alias& alias) const
    {
      return ::sqlpp::as(*this, alias);
    }
  };

  template <typename TableSpec, typename ColumnSpec>
  struct value_type_of<column_t<TableSpec, ColumnSpec>>
  {
    using type = typename ColumnSpec::value_type;
  };

  template <typename TableSpec, typename ColumnSpec>
  struct name_tag_of<column_t<TableSpec, ColumnSpec>>
  {
    using type = typename ColumnSpec::_sqlpp_name_tag;
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_selectable_v<column_t<TableSpec, ColumnSpec>> = true;

  template <typename TableSpec, typename ColumnSpec>
  struct can_be_null<column_t<TableSpec, ColumnSpec>>
  {
    static constexpr auto value = ColumnSpec::can_be_null;
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_read_only_v<column_t<TableSpec, ColumnSpec>> =
      is_read_only_v<std::decay_t<decltype(ColumnSpec::default_value)>>;

  template <typename TableSpec, typename ColumnSpec>
  struct has_default<column_t<TableSpec, ColumnSpec>>
  {
    static constexpr auto value =
        ColumnSpec::can_be_null or not std::is_same_v<decltype(ColumnSpec::default_value), ::sqlpp::none_t>;
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_insert_required_v<column_t<TableSpec, ColumnSpec>> =
      not has_default<column_t<TableSpec, ColumnSpec>>::value;

  template <typename DbConnection, typename TableSpec, typename ColumnSpec>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const column_t<TableSpec, ColumnSpec>& t)
  {
    return to_sql_name(connection, TableSpec{}) + "." + to_sql_name(connection, ColumnSpec{});
  }

}  // namespace sqlpp
