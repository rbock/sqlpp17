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

#include <sqlpp17/core/clause/union.h>
#include <sqlpp17/core/column.h>
#include <sqlpp17/core/result_row.h>
#include <sqlpp17/core/table_spec.h>
#include <sqlpp17/core/type_traits.h>
#include <sqlpp17/core/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename TableSpec, typename ResultRow>
  struct cte_columns_t
  {
    static_assert(wrong<cte_columns_t>, "Invalid arguments for cte_columns_t");
  };

  template <typename TableSpec, typename... ResultColumnSpecs>
  struct cte_columns_t<TableSpec, result_row_t<ResultColumnSpecs...>>
      : member_t<ResultColumnSpecs, column_t<TableSpec, ResultColumnSpecs>>...
  {
  };

  struct flat_t;
  struct recursive_t;

  template <typename CteType, typename TableSpec, typename Statement>
  struct cte_t;

  namespace detail
  {
    template <typename TableSpec, typename... ResultColumnSpecs>
    [[nodiscard]] constexpr auto all_of(const cte_columns_t<TableSpec, ResultColumnSpecs...>& t)
    {
      return std::tuple{column_t<TableSpec, ResultColumnSpecs>{}...};
    }
  }  // namespace detail

  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_union_is_not_recursive_twice, "union*() must not be called twice on a cte");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_cte_union_arg_is_compatible,
                              "union*() arg must have compatible result columns (names and types)");

  template <typename Cte, typename Statement>
  constexpr auto check_cte_union_args([[maybe_unused]] type_t<Cte>, [[maybe_unused]] type_t<Statement>)
  {
    if constexpr (is_cte_recursive_v<Cte>)
    {
      return failed<assert_cte_union_is_not_recursive_twice>{};
    }
    else if constexpr (not result_rows_are_compatible_v<result_row_of_t<Cte>, result_row_of_t<Statement>>)
    {
      return failed<assert_cte_union_arg_is_compatible>{};
    }
    else
      return succeeded{};
  }

  template <typename CteType, typename TableSpec, typename Statement>
  struct cte_t : cte_columns_t<TableSpec, result_row_of_t<Statement>>
  {
    Statement _statement;

    cte_t(Statement statement) : _statement(statement)
    {
    }

    template <typename... Clauses>
    constexpr auto union_all(statement<Clauses...> second_statement)
    {
      if constexpr (constexpr auto _check = check_cte_union_args(type_t<cte_t>{}, type_t<statement<Clauses...>>{});
                    _check)
      {
        auto _union = ::sqlpp::union_all(_statement, second_statement);
        return cte_t<recursive_t, TableSpec, decltype(_union)>(_union);
      }
      else
      {
        return bad_expression_t{_check};
      }
    }

    template <typename... Clauses>
    constexpr auto union_distinct(statement<Clauses...> second_statement)
    {
      if constexpr (constexpr auto _check = check_cte_union_args(type_t<cte_t>{}, type_t<statement<Clauses...>>{});
                    _check)
      {
        auto _union = ::sqlpp::union_distinct(_statement, second_statement);
        return cte_t<recursive_t, TableSpec, decltype(_union)>(_union);
      }
      else
      {
        return bad_expression_t{_check};
      }
    }
  };

  template <typename CteType, typename TableSpec, typename Statement>
  struct is_cte_recursive<cte_t<CteType, TableSpec, Statement>>
      : std::integral_constant<bool, std::is_same_v<CteType, recursive_t>>
  {
  };

  template <typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] constexpr auto all_of(const cte_t<CteType, TableSpec, Statement>& t)
  {
    using _base = cte_columns_t<TableSpec, result_row_of_t<Statement>>;
    return detail::all_of(static_cast<const _base&>(t));
  }

  template <typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] constexpr auto provided_tables_of([[maybe_unused]] type_t<cte_t<CteType, TableSpec, Statement>>)
  {
    return type_vector<TableSpec>();
  }

  template <typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] constexpr auto required_tables_of([[maybe_unused]] type_t<cte_t<CteType, TableSpec, Statement>>)
  {
    return type_set<>();
  }

  template <typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] constexpr auto required_ctes_of(type_t<cte_t<CteType, TableSpec, Statement>> c)
  {
    return provided_tables_of(c);
  };

  template <typename CteType, typename TableSpec, typename Statement>
  struct nodes_of<cte_t<CteType, TableSpec, Statement>>
  {
    using type = type_vector<Statement>;
  };

  template <typename CteType, typename TableSpec, typename Statement>
  struct result_row_of<cte_t<CteType, TableSpec, Statement>>
  {
    using type = result_row_of_t<Statement>;
  };

  template <typename CteType, typename TableSpec, typename Statement>
  constexpr auto is_table_v<cte_t<CteType, TableSpec, Statement>> = true;

  template <typename CteType, typename TableSpec, typename Statement>
  struct is_cte<cte_t<CteType, TableSpec, Statement>> : std::true_type
  {
  };

  template <typename CteType, typename TableSpec, typename Statement>
  struct name_tag_of<cte_t<CteType, TableSpec, Statement>>
  {
    using type = name_tag_of_t<TableSpec>;
  };

  template <typename Context, typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] auto to_full_sql_string(Context& context, const cte_t<CteType, TableSpec, Statement>& t)
  {
    return to_sql_name(context, t) + " AS (" + to_sql_string(context, t._statement) + ")";
  }

  template <typename Context, typename CteType, typename TableSpec, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const cte_t<CteType, TableSpec, Statement>& t)
  {
    return to_sql_name(context, t);
  }
}  // namespace sqlpp
