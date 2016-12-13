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

#include <type_traits>
#include <utility>
#include <sqlpp17/type_set.h>
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  struct boolean_t;
  namespace clause
  {
    struct custom
    {
    };
  }

  struct no_clause
  {
  };

  template <typename T>
  constexpr auto is_tagged_clause_v = false;

  template <typename T>
  constexpr auto is_failed_v = false;

  template <typename T>
  constexpr auto is_failed(const T&)
  {
    return is_failed_v<T>;
  };

  template <typename T>
  constexpr auto clause_tag = no_clause{};

  template <typename T>
  constexpr auto contains_aggregate_v = false;

  template <typename T>
  constexpr auto contains_aggregate(const T&)
  {
    return contains_aggregate_v<T>;
  }

  template <typename Assert, typename T>
  constexpr auto is_alias_v = false;

  template <typename Assert, typename T>
  constexpr auto is_alias(const Assert&, const T&)
  {
    return is_alias_v<Assert, T>;
  }

  template <typename Assert, typename T>
  constexpr auto is_bad_statement_v = false;

  template <typename Assert, typename T>
  constexpr auto is_bad_statement(const Assert&, const T&)
  {
    return is_bad_statement_v<Assert, T>;
  }

  template <typename T>
  constexpr auto is_join_v = false;

  template <typename T>
  constexpr auto is_join(const T&)
  {
    return is_join_v<T>;
  }

  template <typename T>
  constexpr auto is_selectable_v = false;

  template <typename T>
  constexpr auto is_selectable(const T&)
  {
    return is_selectable_v<T>;
  }

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
  constexpr auto is_expression_v = false;

  template <typename T>
  constexpr auto is_expression(const T&)
  {
    return is_expression<T>;
  }

  struct no_value_t
  {
  };

  template <typename T>
  constexpr auto value_type_of_v = no_value_t{};

  template <typename T>
  constexpr auto value_type_of(const T&)
  {
    return value_type_of_v<T>;
  }

  template <typename T>
  constexpr auto is_boolean_v = std::is_same_v<std::decay_t<decltype(value_type_of_v<T>)>, boolean_t>;

  template <typename T>
  constexpr auto is_boolean(const T&)
  {
    return is_boolean_v<T>;
  }

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
  constexpr auto required_tables_of_v = type_set_t<>();

  template <typename T>
  constexpr auto required_tables_of(const T&)
  {
    return required_tables_of_v<T>;
  }

  template <typename T>
  constexpr auto provided_tables_of_v = type_set<>();

  template <typename T>
  constexpr auto provided_tables_of(const T&)
  {
    return provided_tables_of_v<T>;
  }

  template <typename T>
  struct char_sequence_of_impl
  {
    static_assert(wrong<T>, "No valid specialization of get_name_type_of found");
  };

  template <typename T>
  using char_sequence_of_t = typename char_sequence_of_impl<T>::type;

  template <typename T>
  constexpr auto char_sequence_of(const T&)
  {
    return char_sequence_of_t<T>{};
  }

  template <typename T>
  constexpr auto provided_table_names_of_v = ::sqlpp::transform<char_sequence_of_t>(provided_tables_of_v<T>);

  template <typename T>
  constexpr auto provided_table_names_of(const T&)
  {
    return provided_table_names_of_v<T>;
  }

  template <typename T>
  using name_of = T;
}
