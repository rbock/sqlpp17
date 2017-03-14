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

#include <tuple>
#include <vector>

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/separator.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct select_columns
    {
    };
  }

  template <typename... Columns>
  struct select_columns_t
  {
    std::tuple<Columns...> _columns;
  };

  template <typename... ColumnTypes>
  struct dynamic_select_columns_t
  {
    std::vector<std::variant<ColumnTypes...>> _columns;
  };

  template <typename... Columns>
  constexpr auto is_result_clause_v<select_columns_t<Columns...>> = true;

  template <typename... ColumnTypes>
  constexpr auto is_result_clause_v<dynamic_select_columns_t<ColumnTypes...>> = true;

  template <typename... Columns>
  constexpr auto clause_tag<select_columns_t<Columns...>> = clause::select_columns{};

  template <typename... ColumnTypes>
  constexpr auto clause_tag<dynamic_select_columns_t<ColumnTypes...>> = clause::select_columns{};

  template <typename... Columns, typename Statement>
  class clause_base<select_columns_t<Columns...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<select_columns_t<Columns...>, OtherStatement>& s) : _columns(s._columns)
    {
    }

    clause_base(const select_columns_t<Columns...>& f) : _columns(f._columns)
    {
    }

    std::tuple<Columns...> _columns;
  };

  template <typename... ColumnTypes, typename Statement>
  class clause_base<dynamic_select_columns_t<ColumnTypes...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<dynamic_select_columns_t<ColumnTypes...>, OtherStatement>& s) : _columns(s._columns)
    {
    }

    clause_base(const dynamic_select_columns_t<ColumnTypes...>& f) : _columns(f._columns)
    {
    }

    std::vector<std::variant<ColumnTypes...>> _columns;
  };

#warning : Need to implement result_row_t for real...

  template <typename... LeftColumnSpecs, typename... RightColumnSpecs>
  constexpr auto
      result_rows_are_compatible_v<result_row_t<LeftColumnSpecs...>,
                                   result_row_t<RightColumnSpecs...>,
                                   std::enable_if_t<sizeof...(LeftColumnSpecs) == sizeof...(RightColumnSpecs)>> =
          (true && ... && column_specs_are_compatible_v<LeftColumnSpecs, RightColumnSpecs>);

  template <typename Alias, typename ValueType, tag::type Tags>
  struct column_spec
  {
    using _alias_t = Alias;
    using value_type = ValueType;
    static constexpr auto tags = Tags;
  };

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto value_type_of_v<column_spec<Alias, ValueType, Tags>> = ValueType{};

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto can_be_null_v<column_spec<Alias, ValueType, Tags>> = (Tags & tag::can_be_null);

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto null_is_trivial_value_v<column_spec<Alias, ValueType, Tags>> = (Tags & tag::null_is_trivial_value);

  template <typename LeftAlias,
            typename LeftValueType,
            tag::type LeftTags,
            typename RightAlias,
            typename RightValueType,
            tag::type RightTags>
  constexpr auto column_specs_are_compatible_v<column_spec<LeftAlias, LeftValueType, LeftTags>,
                                               column_spec<RightAlias, RightValueType, RightTags>> =
      std::is_same_v<make_char_sequence<LeftAlias::name>, make_char_sequence<RightAlias::name>>&&
          std::is_same_v<LeftValueType, RightValueType>;

  template <typename Column>
  using make_column_spec = column_spec<typename Column::_alias_t,
                                       value_type_of_t<Column>,
                                       can_be_null_v<Column> | null_is_trivial_value_v<Column>>;

  template <typename... Columns, typename Statement>
  class result_base<select_columns_t<Columns...>, Statement>
  {
  public:
    using result_row_t = result_row_t<make_column_spec<Columns>...>;

    template <typename Connection>
    [[nodiscard]] auto run(Connection& connection) const
    {
      return connection.select(Statement::of(this), result_row_t{});
    }
  };

#warning : The dynamic vector variant is missing

  template <typename Context, typename... Columns, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<select_columns_t<Columns...>, Statement>& t)
  {
    auto separate = detail::separator<Context>{context, ", "};
    context << ' ';
    return (context << ... << separate(std::get<Columns>(t._columns)));
  }

  template <typename Context, typename... ColumnTypes, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<dynamic_select_columns_t<ColumnTypes...>, Statement>& t)
  {
#warning : Need to print for real
    return context;
  }
}
