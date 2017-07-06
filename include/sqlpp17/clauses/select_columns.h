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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/select_column_printer.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/statement.h>
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

  template <typename... Columns>
  constexpr auto is_result_clause_v<select_columns_t<Columns...>> = true;

  template <typename... Columns>
  constexpr auto clause_tag<select_columns_t<Columns...>> = clause::select_columns{};

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
  constexpr auto can_be_null_v<column_spec<Alias, ValueType, Tags>> = !!(Tags & tag::can_be_null);

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto null_is_trivial_value_v<column_spec<Alias, ValueType, Tags>> = !!(Tags & tag::null_is_trivial_value);

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

  template <typename Statement, typename OptColumn>
  struct make_column_spec
  {
    using _opt_column_t = OptColumn;
    using _column_t = remove_optional_t<OptColumn>;
    using _alias_t = typename _column_t::_alias_t;
    using _value_t = value_type_of_t<_column_t>;

    static constexpr auto _outer_tables = outer_tables_of_v<Statement>;
    static constexpr auto _column_can_be_null = can_be_null_v<_column_t>;
    static constexpr auto _column_is_optional = is_optional_v<_opt_column_t>;
    static constexpr auto _column_is_in_outer_table = can_be_null_v<_column_t>;
    static constexpr auto _can_be_null_tag =
        tag_if_v<tag::can_be_null, _column_can_be_null | _column_is_optional | _column_is_in_outer_table>;

    static constexpr auto _null_is_trivial_value = null_is_trivial_value_v<_opt_column_t>;

    static constexpr auto _null_is_trivial_value_tag = tag_if_v<tag::null_is_trivial_value, _null_is_trivial_value>;
    using type = column_spec<_alias_t, _value_t, _can_be_null_tag | _null_is_trivial_value_tag>;
  };

  template <typename Statement, typename OptColumn>
  using make_column_spec_t = typename make_column_spec<Statement, OptColumn>::type;

  SQLPP_WRAPPED_STATIC_ASSERT(assert_selected_columns_all_aggregates_or_none,
                              "columns need to be either all aggregates or all non-aggregates");

  template <typename Db, typename... Columns, typename... Clauses>
  constexpr auto check_clause_executable(
      const type_t<clause_base<select_columns_t<Columns...>, statement<Clauses...>>>& t)
  {
    using known_aggregates_t = decltype((::sqlpp::type_set() | ... | provided_aggregates_v<Clauses>));

    constexpr auto all_aggregates = (true && ... && recursive_is_aggregate<known_aggregates_t, Columns>());
    constexpr auto no_aggregates = (true && ... && recursive_is_non_aggregate<known_aggregates_t, Columns>());

    if constexpr (!(all_aggregates || !no_aggregates))
    {
      return failed<assert_selected_columns_all_aggregates_or_none>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename... Columns, typename Statement>
  class result_base<select_columns_t<Columns...>, Statement>
  {
  public:
    using result_row_t = result_row_t<make_column_spec_t<Statement, Columns>...>;

    template <typename Connection>
    [[nodiscard]] auto _run(Connection& connection) const
    {
      return connection.select(Statement::of(this), result_row_t{});
    }
  };

  template <typename Context, typename... Columns, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<select_columns_t<Columns...>, Statement>& t)
  {
    auto print = detail::select_column_printer<Context>{context, ", "};
    context << ' ';
    (..., print(std::get<Columns>(t._columns)));
    return context;
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_not_empty,
                              "select columns() must be called with at least one argument");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_are_selectable,
                              "select columns() args must be selectable (i.e. named expressions)");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_columns_args_have_unique_names,
                              "select columns() args must have unique names");

  template <typename... T>
  constexpr auto check_select_columns_arg()
  {
    if constexpr (sizeof...(T) == 0)
    {
      return failed<assert_select_columns_args_not_empty>{};
    }
    else if constexpr (!(true && ... && is_selectable_v<T>))
    {
      return failed<assert_select_columns_args_are_selectable>{};
    }
    else if constexpr (type_set<char_sequence_of_t<T>...>().size() != sizeof...(T))
    {
      return failed<assert_select_columns_args_have_unique_names>{};
    }
    else
      return succeeded{};
  }

  struct no_select_columns_t
  {
  };

  template <typename Statement>
  class clause_base<no_select_columns_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_select_columns_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(Columns... columns) const
    {
      constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>();
      if constexpr (check)
      {
        return Statement::of(this).template replace_clause<no_select_columns_t>(
            select_columns_t<Columns...>{std::make_tuple(columns...)});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Columns>
    [[nodiscard]] constexpr auto columns(std::tuple<Columns...> columns) const
    {
      constexpr auto check = check_select_columns_arg<remove_optional_t<Columns>...>();
      if constexpr (check)
      {
        return Statement::of(this).template replace_clause<no_select_columns_t>(select_columns_t<Columns...>{columns});
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_select_columns_t, Statement>& t)
  {
    return context;
  }

  template <typename... Columns>
  [[nodiscard]] constexpr auto select_columns(Columns&&... columns)
  {
    return statement<no_select_columns_t>{}.columns(std::forward<Columns>(columns)...);
  }
}
