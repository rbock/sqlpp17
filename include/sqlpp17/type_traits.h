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

#include <cstdint>
#include <type_traits>

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <sqlpp17/char_sequence.h>
#include <sqlpp17/type_set.h>
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  struct none_t
  {
  };

  struct name_tag_base
  {
  };

  struct spec_base
  {
  };

  template <typename T>
  constexpr auto is_auto_value_v = false;

  struct auto_increment_t
  {
  };

  template <>
  constexpr auto is_auto_value_v<auto_increment_t> = true;

  template <typename T>
  struct type_t
  {
  };

  template <typename T>
  constexpr auto type_v = type_t<T>{};

  struct boolean_t;
  struct no_clause
  {
  };

  template <typename T>
  constexpr auto sub_expression_set_v = type_set();

  template <typename T>
  constexpr auto is_aggregate_v = false;

  template <typename T>
  struct can_be_null
  {
    static constexpr auto value = false;
  };

  template <typename T>
  constexpr auto can_be_null_v = can_be_null<T>::value;

  template <typename KnownAggregatesSet, typename... Ts>
  constexpr auto recursive_is_aggregate(const type_set_t<Ts...>&)
  {
    return (true && ... && recursive_is_aggregate<KnownAggregatesSet, Ts>());
  }

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_is_aggregate()
  {
    return (is_aggregate_v<T> || KnownAggregatesSet::template count<T>()) &&
           recursive_is_aggregate<KnownAggregatesSet>(sub_expression_set_v<T>);
  }

  // constant values and type-erased values could be treated as both,
  // aggregate and non-aggregate
  template <typename T>
  constexpr auto is_non_aggregate_v = false;

  template <typename KnownAggregatesSet, typename... Ts>
  constexpr auto recursive_is_non_aggregate(const type_set_t<Ts...>&)
  {
    return (true && ... && recursive_is_non_aggregate<KnownAggregatesSet, Ts>());
  }

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_is_non_aggregate()
  {
    return is_non_aggregate_v<T> && (KnownAggregatesSet::template count<T>() == 0) &&
           recursive_is_non_aggregate<KnownAggregatesSet>(sub_expression_set_v<T>);
  }

  template <typename T>
  constexpr auto provided_aggregates_v = type_set_t<>{};

  template <typename T>
  constexpr auto is_insert_required_v = false;

  template <typename T>
  struct is_insert_required
  {
    static constexpr auto value = is_insert_required_v<T>;
  };

  template <typename T>
  constexpr auto is_failed_v = false;

  template <typename T>
  constexpr auto is_failed(const T&)
  {
    return is_failed_v<T>;
  }

  template <typename T>
  constexpr auto clause_tag = no_clause{};

  template <typename T>
  constexpr auto is_clause_v = !std::is_same_v<std::decay_t<decltype(clause_tag<T>)>, no_clause>;

  template <typename T>
  constexpr auto is_clause(const T&)
  {
    return is_clause_v<T>;
  }

  template <typename T>
  constexpr auto contains_aggregate_v = false;

  template <typename T>
  constexpr auto contains_aggregate(const T&)
  {
    return contains_aggregate_v<T>;
  }

  template <typename T>
  constexpr auto is_alias_v = false;

  template <typename T>
  constexpr auto is_alias(const T&)
  {
    return is_alias_v<T>;
  }

  template <typename T>
  constexpr auto is_assignment_v = false;

  template <typename T>
  constexpr auto is_assignment(const T&)
  {
    return is_assignment_v<T>;
  }

  template <typename T>
  constexpr auto is_column_v = false;

  template <typename T>
  constexpr auto is_column(const T&)
  {
    return is_column_v<T>;
  }

  template <typename T>
  constexpr auto is_statement_v = false;

  template <typename T>
  constexpr auto is_statement(const T&)
  {
    return is_statement_v<T>;
  }

  template <typename T, typename Enable = void>
  constexpr auto has_result_row_v = false;

  template <typename T>
  constexpr auto has_result_row_v<T, std::void_t<typename T::_result_row_t>> = true;

  template <typename T>
  constexpr auto has_result_row(const T&)
  {
    return has_result_row_v<T>;
  }

  template <typename Left, typename Right>
  struct result_rows_are_compatible
  {
    static constexpr auto result_rows_are_compatible_v = false;
  };

  template <typename Left, typename Right>
  constexpr auto result_rows_are_compatible_v = result_rows_are_compatible<Left, Right>::value;

  template <typename T>
  struct result_row_of
  {
    using type = std::nullptr_t;
  };

  template <typename T>
  using result_row_of_t = typename result_row_of<T>::type;

  template <typename Assert, typename T>
  constexpr auto is_bad_statement_v = false;

  template <typename Assert, typename T>
  constexpr auto is_bad_statement(const Assert&, const T&)
  {
    return is_bad_statement_v<Assert, T>;
  }

  template <typename T>
  struct is_row_result
  {
    static constexpr auto value = false;
  };

  template <typename T>
  static constexpr auto is_row_result_v = is_row_result<T>::value;

  template <typename T>
  constexpr auto is_join_v = false;

  template <typename T>
  constexpr auto is_join(const T&)
  {
    return is_join_v<T>;
  }

  template <typename T>
  constexpr auto is_result_clause_v = false;

  template <typename T>
  constexpr auto is_result_clause(const T&)
  {
    return is_result_clause_v<T>;
  }

  template <typename T>
  constexpr auto is_selectable_v = false;

  template <typename T>
  constexpr auto is_selectable(const T&)
  {
    return is_selectable_v<T>;
  }

  template <typename T>
  constexpr auto is_sort_order_v = false;

  template <typename T>
  constexpr auto is_table_v = false;

  template <typename T>
  constexpr auto is_table(const T&)
  {
    return is_table_v<T>;
  }

  template <typename T>
  constexpr auto is_read_only_table_v = false;

  template <typename T>
  constexpr auto is_read_only_table(const T&)
  {
    return is_read_only_table_v<T>;
  }

  template <typename T>
  constexpr auto is_conditionless_join_v = false;

  template <typename T>
  constexpr auto is_conditionless_join(const T&)
  {
    return is_conditionless_join<T>;
  }

  template <typename T>
  struct has_default
  {
    static constexpr auto value = false;
  };

  template <typename T>
  constexpr auto is_select_flag_v = false;

  template <typename T>
  constexpr auto is_select_flag(const T&)
  {
    return is_select_flag<T>;
  }

  template <typename T>
  struct value_type_of
  {
    using type = none_t;
  };

  template <typename T, typename Enable = void>
  using value_type_of_t = typename value_type_of<T>::type;

  struct no_value_t
  {
  };

  template <typename T>
  constexpr auto is_expression_v = not std::is_same_v<value_type_of_t<T>, none_t>;

  template <typename T>
  constexpr auto is_expression(const T&)
  {
    return is_expression<T>;
  }

  template <typename T, typename Enable = void>
  constexpr auto table_spec_of_v = no_value_t{};

  template <typename T>
  using table_spec_of_t = std::decay_t<decltype(table_spec_of_v<T>)>;

  template <typename T>
  constexpr auto table_spec_of(const T&)
  {
    return table_spec_of_v<T>;
  }

  template <typename T>
  struct column_of
  {
    using type = no_value_t;
  };

  template <typename T>
  using column_of_t = typename column_of<T>::type;

  template <typename T>
  constexpr auto is_optional_v = false;

  template <typename T>
  constexpr auto is_optional_v<std::optional<T>> = true;

  template <typename T>
  struct is_optional
  {
    static constexpr auto value = is_optional_v<T>;
  };

  template <typename T>
  constexpr auto is_optional_f(const T&)
  {
    return is_optional_v<T>;
  }

  template <typename T>
  struct is_value_type_optional
  {
    static constexpr auto value = is_optional_v<value_type_of<T>>;
  };

  template <typename T>
  struct remove_optional
  {
    using type = T;
  };

  template <typename T>
  struct remove_optional<std::optional<T>>
  {
    using type = T;
  };

  template <typename T>
  using remove_optional_t = typename remove_optional<T>::type;

  template <typename T>
  struct add_optional
  {
    using type = std::optional<T>;
  };

  template <typename T>
  using add_optional_t = typename add_optional<T>::type;

  template <typename T>
  decltype(auto) get_value(const T& t)
  {
    return t;
  }

  template <typename T>
  decltype(auto) get_value(const std::optional<T>& t)
  {
    return t.value();
  }

  template <typename T>
  auto has_value(const T& t) -> bool
  {
    if constexpr (sqlpp::is_optional_v<T>)
    {
      return t.has_value();
    }

    return true;
  }

  template <typename... Ts>
  constexpr auto any_has_value(const std::tuple<Ts...>& t) -> bool
  {
    return (false || ... || has_value(std::get<Ts>(t)));
  }
  template <typename T>
  constexpr auto is_boolean_v = std::is_same_v<T, bool>;

  template <typename T>
  constexpr auto has_boolean_value_v =
      is_boolean_v<remove_optional_t<T>> or is_boolean_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename T>
  constexpr auto is_boolean(const T&)
  {
    return is_boolean_v<T>;
  }

  struct integral_t;

  struct numeric_t;

  struct text_t;

  template <typename T>
  constexpr auto is_integral_v = std::is_integral_v<T>;

  template <>
  constexpr auto is_integral_v<char> = false;  // char is text

  template <>
  constexpr auto is_integral_v<bool> = false;  // bool is boolean

  template <>
  constexpr auto is_integral_v<integral_t> = true;

  template <typename T>
  constexpr auto has_integral_value_v =
      is_integral_v<remove_optional_t<T>> or is_integral_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename T>
  constexpr auto is_numeric_v = is_integral_v<T> || std::is_floating_point_v<T>;

  template <>
  constexpr auto is_numeric_v<numeric_t> = true;

  template <typename T>
  constexpr auto has_numeric_value_v =
      is_numeric_v<remove_optional_t<T>> or is_numeric_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename T>
  constexpr auto is_text_v = false;

  template <>
  constexpr auto is_text_v<char> = true;

  template <>
  constexpr auto is_text_v<const char*> = true;

  template <>
  constexpr auto is_text_v<std::string> = true;

  template <>
  constexpr auto is_text_v<std::string_view> = true;

  template <typename T>
  constexpr auto has_text_value_v = is_text_v<remove_optional_t<T>> or is_text_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename L, typename R, typename Enable = void>
  constexpr auto are_values_comparable_v = false;

  template <typename L, typename R>
  constexpr auto are_values_comparable_v<L, R, std::enable_if_t<has_numeric_value_v<L> and has_numeric_value_v<R>>> =
      true;

  template <typename L, typename R>
  constexpr auto are_values_comparable_v<L, R, std::enable_if_t<has_text_value_v<L> and has_text_value_v<R>>> = true;

  template <typename L, typename R>
  constexpr auto are_values_comparable_v<L, R, std::enable_if_t<has_boolean_value_v<L> and has_boolean_value_v<R>>> =
      true;

  template <typename T>
  constexpr auto is_conditionless_dynamic_join = false;

  template <typename T>
  constexpr auto is_dynamic_join = false;

  template <typename T>
  constexpr auto requires_braces_v = false;

  template <typename T>
  constexpr auto requires_braces(const T&)
  {
    return requires_braces_v<T>;
  }

  template <typename T>
  constexpr auto parameters_of = type_set_t<>();

  template <typename T>
  constexpr auto required_columns_of_v = type_set_t<>();

  template <typename T>
  constexpr auto required_columns_of(const T&)
  {
    return required_columns_of_v<T>;
  }

  template <typename T>
  constexpr auto default_columns_of_v = type_set_t<>();

  template <typename T>
  constexpr auto default_columns_of(const T&)
  {
    return default_columns_of_v<T>;
  }

  template <typename T>
  constexpr auto can_be_null_columns_of_v = type_set_t<>();

  template <typename T>
  constexpr auto can_be_null_columns_of(const T&)
  {
    return can_be_null_columns_of_v<T>;
  }

  template <typename T>
  constexpr auto required_cte_names_of_v = type_set_t<>();

  template <typename T>
  constexpr auto required_cte_names_of(const T&)
  {
    return required_cte_names_of_v<T>;
  }

  template <typename T>
  constexpr auto required_insert_columns_of_v = type_set_t<>();

  template <typename T>
  constexpr auto required_insert_columns_of(const T&)
  {
    return required_insert_columns_of_v<T>;
  }

  template <typename T>
  constexpr auto table_names_of_v = type_set<>();

  template <typename T>
  constexpr auto table_names_of(const T&)
  {
    return table_names_of_v<T>;
  }

  template <typename T>
  constexpr auto columns_of_v = type_set<>();

  template <typename T>
  constexpr auto columns_of(const T&)
  {
    return columns_of_v<T>;
  }

  template <typename T>
  constexpr auto has_result_rows_v = false;

  template <typename T>
  struct cpp_type
  {
    using type = T;
  };

  template <typename T>
  using cpp_type_t = typename cpp_type<T>::type;

  template <typename T>
  constexpr auto char_sequence_of_v = char_sequence<>{};

  template <typename T>
  using char_sequence_of_t = std::decay_t<decltype(char_sequence_of_v<T>)>;

  template <typename T>
  constexpr auto char_sequence_of(const T&)
  {
    return char_sequence_of_t<T>{};
  }

  template <typename T>
  struct name_tag_of
  {
    using type = none_t;
  };

  template <typename T>
  using name_tag_of_t = typename name_tag_of<T>::type;

}  // namespace sqlpp
