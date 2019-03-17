#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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
#include <sqlpp17/as_base.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/to_sql_name.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename TableSpec, typename ColumnSpec>
  class column_t : public as_base<column_t<TableSpec, ColumnSpec>>
  {
    static_assert(std::is_base_of_v<::sqlpp::spec_base, TableSpec>);
    static_assert(std::is_base_of_v<::sqlpp::spec_base, ColumnSpec>);

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
  struct column_spec_of<column_t<TableSpec, ColumnSpec>>
  {
    using type = ColumnSpec;
  };

  template <typename TableSpec, typename ColumnSpec>
  struct table_spec_of<column_t<TableSpec, ColumnSpec>>
  {
    using type = TableSpec;
  };

  template <typename TableSpec, typename ColumnSpec>
  struct can_be_null<column_t<TableSpec, ColumnSpec>>
  {
    static constexpr auto value = ColumnSpec::can_be_null;
  };

  template <typename TableSpec, typename ColumnSpec>
  constexpr auto is_read_only_v<column_t<TableSpec, ColumnSpec>> = ColumnSpec::has_auto_increment;

  template <typename TableSpec, typename ColumnSpec>
  struct has_default<column_t<TableSpec, ColumnSpec>>
  {
    static constexpr auto value = ColumnSpec::has_default_value or ColumnSpec::can_be_null or ColumnSpec::has_auto_increment;
  };

  template <typename TableSpec, typename ColumnSpec>
  struct is_insert_required<column_t<TableSpec, ColumnSpec>>
      : std::integral_constant<bool, not has_default_v<column_t<TableSpec, ColumnSpec>>>
  {
  };

  template <typename TableSpec, typename ColumnSpec>
  [[nodiscard]] constexpr auto required_tables_of([[maybe_unused]] type_t<column_t<TableSpec, ColumnSpec>>)
  {
    return type_set<TableSpec>();
  }

  template <typename Context, typename TableSpec, typename ColumnSpec>
  [[nodiscard]] auto to_sql_string(Context& context, const column_t<TableSpec, ColumnSpec>& t)
  {
    return to_sql_name(context, TableSpec{}) + "." + to_sql_name(context, ColumnSpec{});
  }

}  // namespace sqlpp
