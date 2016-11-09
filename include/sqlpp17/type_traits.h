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

namespace sqlpp
{
  template <typename T>
  constexpr auto is_join_v = false;

  template <typename T>
  constexpr auto is_join(const T&)
  {
    return is_join_v<T>;
  };

  template <typename T>
  constexpr auto is_selectable_v = false;

  template <typename T>
  constexpr auto is_selectable(const T&)
  {
    return is_selectable_v<T>;
  };

  template <typename T>
  constexpr auto is_table_v = false;

  template <typename T>
  constexpr auto is_table(const T&)
  {
    return is_table_v<T>;
  };

  template <typename T>
  constexpr auto is_boolean = false;

  template <typename T>
  constexpr auto is_expression = false;

  template <typename T>
  constexpr auto is_conditionless_join = false;

  template <typename T>
  constexpr auto is_conditionless_dynamic_join = false;

  template <typename T>
  constexpr auto is_dynamic_join = false;

  template <typename T>
  constexpr auto requires_braces = false;

  template <typename T>
  constexpr auto parameters_of = type_set_t<>();

  template <typename T>
  constexpr auto required_tables_of = type_set_t<>();

  template <typename T>
  constexpr auto provided_tables_of_v = type_set<>();

  template <typename T>
  constexpr auto provided_tables_of(const T&)
  {
    return provided_tables_of_v<T>;
  }

  template <typename T>
  constexpr auto provided_table_names_of = type_set_t<>();

  template <typename T>
  using name_of = T;
}
