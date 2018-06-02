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

#include <cstdint>
#include <type_traits>

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <sqlpp17/char_sequence.h>
#include <sqlpp17/type_hash.h>
#include <sqlpp17/type_set.h>
#include <sqlpp17/type_vector.h>
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
  constexpr auto is_read_only_v = false;

  struct auto_increment_t
  {
  };

  template <>
  constexpr auto is_read_only_v<auto_increment_t> = true;

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
  struct nodes_of
  {
    using type = type_vector<>;
  };

  template <typename T>
  using nodes_of_t = typename nodes_of<T>::type;

  template <typename T>
  struct can_be_null
  {
    static constexpr auto value = false;
  };

  template <typename T>
  constexpr auto can_be_null_v = can_be_null<T>::value;

  template <typename T>
  struct provided_aggregates_of
  {
    static constexpr auto value = type_set_t<>{};
  };

  template <typename T>
  inline constexpr auto provided_aggregates_of_v = provided_aggregates_of<T>::value;

  template <typename T>
  struct is_insert_required : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_insert_required_v = is_insert_required<T>::value;

  template <typename T>
  struct is_insert_clause : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_insert_clause_v = is_insert_clause<T>::value;

  template <typename T>
  struct is_delete_clause : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_delete_clause_v = is_delete_clause<T>::value;

  template <typename T>
  struct is_update_clause : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_update_clause_v = is_update_clause<T>::value;

  template <typename T>
  struct is_select_clause : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_select_clause_v = is_select_clause<T>::value;

  template <typename T>
  struct is_execute_clause : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_execute_clause_v = is_execute_clause<T>::value;

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

  template <typename T>
  struct result_row_of
  {
    using type = none_t;
  };

  template <typename T>
  using result_row_of_t = typename result_row_of<T>::type;

  template <typename T>
  constexpr auto has_result_row_v = not std::is_same_v<result_row_of_t<T>, none_t>;

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
  constexpr auto is_bad_expression_v = false;

  template <typename Assert, typename T>
  constexpr auto is_specific_bad_expression_v = false;

  template <typename T>
  constexpr auto is_bad_expression()
  {
    return is_bad_expression_v<T>;
  }

  template <typename Assert, typename T>
  constexpr auto is_specific_bad_expression()
  {
    return is_specific_bad_expression_v<Assert, T>;
  }

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
  constexpr auto is_sort_order_v = false;

  template <typename T>
  constexpr auto is_table_v = false;

  template <typename T>
  constexpr auto is_table(const T&)
  {
    return is_table_v<T>;
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
  inline constexpr auto has_default_v = has_default<T>::value;

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

  template <typename T>
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

  template <typename... Ts>
  using check_for_expression = std::enable_if_t<(false or ... or is_expression_v<Ts>)>;

  template <typename T>
  constexpr auto is_aggregate_v = false;

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_is_aggregate();

  template <typename KnownAggregatesSet, typename... Ts>
  constexpr auto recursive_is_aggregate(const type_vector<Ts...>&)
  {
    return (true && ... && recursive_is_aggregate<KnownAggregatesSet, Ts>());
  }

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_is_aggregate()
  {
    return not is_expression_v<T> or is_aggregate_v<T> or KnownAggregatesSet::template count<T>() or
           (not nodes_of_t<T>::empty() and recursive_is_aggregate<KnownAggregatesSet>(nodes_of_t<T>{}));
  }

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_contains_aggregate();

  template <typename KnownAggregatesSet, typename... Ts>
  constexpr auto recursive_contains_aggregate(const type_vector<Ts...>&)
  {
    return (false or ... or recursive_contains_aggregate<KnownAggregatesSet, Ts>());
  }

  template <typename KnownAggregatesSet, typename T>
  constexpr auto recursive_contains_aggregate()
  {
    return is_aggregate_v<T> or KnownAggregatesSet::template count<T>() or
           (not nodes_of_t<T>::empty() and recursive_contains_aggregate<KnownAggregatesSet>(nodes_of_t<T>{}));
  }

  template <typename T>
  struct column_spec_of
  {
    static_assert(wrong<T>, "Missing specialization");
  };

  template <typename T>
  using column_spec_of_t = typename column_spec_of<T>::type;

  template <typename T>
  struct table_spec_of
  {
    static_assert(wrong<T>, "Missing specialization");
  };

  template <typename T>
  using table_spec_of_t = typename table_spec_of<T>::type;

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

  template <>
  constexpr auto is_boolean_v<std::nullopt_t> = true;

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
  constexpr auto is_integral_v<std::nullopt_t> = true;

  template <>
  constexpr auto is_integral_v<integral_t> = true;

  template <typename T>
  constexpr auto has_integral_value_v =
      is_integral_v<remove_optional_t<T>> or is_integral_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename T>
  constexpr auto is_numeric_v = is_integral_v<T> || std::is_floating_point_v<T>;

  template <>
  constexpr auto is_numeric_v<numeric_t> = true;

  template <>
  constexpr auto is_numeric_v<std::nullopt_t> = true;

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

  template <>
  constexpr auto is_text_v<std::nullopt_t> = true;

  template <typename T>
  constexpr auto has_text_value_v = is_text_v<remove_optional_t<T>> or is_text_v<remove_optional_t<value_type_of_t<T>>>;

  template <typename L, typename R, typename Enable = void>
  struct values_are_compatible : std::false_type
  {
  };

  template <typename L, typename R>
  struct values_are_compatible<L, R, std::enable_if_t<has_numeric_value_v<L> and has_numeric_value_v<R>>>
      : std::true_type
  {
  };

  template <typename L, typename R>
  struct values_are_compatible<L, R, std::enable_if_t<has_text_value_v<L> and has_text_value_v<R>>> : std::true_type
  {
  };

  template <typename L, typename R>
  struct values_are_compatible<L, R, std::enable_if_t<has_boolean_value_v<L> and has_boolean_value_v<R>>>
      : std::true_type
  {
  };

  template <typename L, typename R>
  inline constexpr auto values_are_compatible_v = values_are_compatible<L, R>::value;

  template <typename T>
  constexpr auto is_conditionless_dynamic_join = false;

  template <typename T>
  constexpr auto is_dynamic_join = false;

  template <typename T>
  struct requires_braces : std::false_type
  {
  };

  template <typename T>
  constexpr auto requires_braces_v = requires_braces<T>::value;

  template <typename T>
  struct parameters_of
  {
    using type = typename parameters_of<nodes_of_t<T>>::type;
  };

  template <typename T>
  using parameters_of_t = typename parameters_of<T>::type;

  template <typename... T>
  struct parameters_of<type_vector<T...>>
  {
    using type = decltype((type_vector<>{} + ... + parameters_of_t<T>{}));
  };

  template <typename... T>
  [[nodiscard]] constexpr auto required_tables_of(type_vector<T...>)
  {
    return (type_set() | ... | required_tables_of(type_t<T>{}));
  }

  template <typename T>
  [[nodiscard]] constexpr auto required_tables_of(type_t<T>)
  {
    return required_tables_of(nodes_of_t<T>{});
  }

  template <typename T>
  constexpr auto required_tables_of_v = required_tables_of(type_t<T>{});

  template <typename... T>
  [[nodiscard]] constexpr auto provided_tables_of(type_vector<T...>)
  {
    return (type_set() | ... | provided_tables_of(type_t<T>{}));
  }

  template <typename T>
  [[nodiscard]] constexpr auto provided_tables_of(type_t<T>)
  {
    return provided_tables_of(nodes_of_t<T>{});
  }

  template <typename T>
  constexpr auto provided_tables_of_v = provided_tables_of(type_t<T>{});

  template <typename... T>
  [[nodiscard]] constexpr auto required_ctes_of(type_vector<T...>)
  {
    return (type_set() | ... | required_ctes_of(type_t<T>{}));
  }

  template <typename T>
  [[nodiscard]] constexpr auto required_ctes_of(type_t<T>)
  {
    return required_ctes_of(nodes_of_t<T>{});
  }

  template <typename T>
  constexpr auto required_ctes_of_v = required_ctes_of(type_t<T>{});

  template <typename... T>
  [[nodiscard]] constexpr auto provided_ctes_of(type_vector<T...>)
  {
    return (type_set() | ... | provided_ctes_of(type_t<T>{}));
  }

  template <typename T>
  [[nodiscard]] constexpr auto provided_ctes_of(type_t<T>)
  {
    return provided_ctes_of(nodes_of_t<T>{});
  }

  template <typename T>
  constexpr auto provided_ctes_of_v = provided_ctes_of(type_t<T>{});

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
  constexpr auto required_insert_columns_of_v = type_set_t<>();

  template <typename T>
  constexpr auto required_insert_columns_of_f(const T&)
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
  struct cpp_type
  {
    using type = T;
  };

  template <typename T>
  using cpp_type_t = typename cpp_type<T>::type;

  namespace detail
  {
    template <typename T>
    auto get_name_tag_of()
    {
      if constexpr (std::is_base_of_v<::sqlpp::name_tag_base, T>)
      {
        return T{};
      }
      else if constexpr (std::is_base_of_v<::sqlpp::spec_base, T>)
      {
        return typename T::_sqlpp_name_tag{};
      }
      else
      {
        return none_t{};
      }
    }
  }  // namespace detail

  template <typename T>
  struct name_tag_of
  {
    using type = decltype(detail::get_name_tag_of<T>());
  };

  template <typename T>
  using name_tag_of_t = typename name_tag_of<T>::type;

  template <typename T>
  struct char_sequence_of
  {
    static_assert(not std::is_same_v<name_tag_of_t<T>, none_t>, "Invalid use of char_sequence_of");
    using type = make_char_sequence_t<name_tag_of_t<T>::name>;
  };

  template <typename T>
  using char_sequence_of_t = typename char_sequence_of<T>::type;

  template <typename T>
  constexpr auto char_sequence_of_v = char_sequence_of_t<T>{};

  template <typename T>
  struct is_selectable : std::integral_constant<bool,
                                                not std::is_same_v<value_type_of_t<T>, none_t> and
                                                    not std::is_same_v<name_tag_of_t<T>, none_t>>
  {
  };

  template <typename T>
  inline constexpr auto is_selectable_v = is_selectable<T>::value;

  template <typename T>
  struct is_cte : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_cte_v = is_cte<T>::value;

  template <typename T>
  struct is_cte_recursive : std::false_type
  {
  };

  template <typename T>
  inline constexpr auto is_cte_recursive_v = is_cte_recursive<T>::value;

}  // namespace sqlpp
