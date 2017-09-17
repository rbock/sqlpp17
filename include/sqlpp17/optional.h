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

#include <optional>
#include <tuple>

#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename T>
  constexpr auto is_optional_v<std::optional<T>> = true;

  template <typename Factory>
  auto make_optional_if(bool condition, const Factory& factory)
  {
    using T = decltype(factory());
    return condition ? std::optional<T>{} : std::make_optional(factory());
  }

  template <typename T>
  auto make_optional_expr(bool condition, T value)
  {
    return condition ? std::optional<T>{} : std::make_optional(value);
  }

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
}
