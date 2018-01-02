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

#include <sqlpp17/operator.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename T>
  struct expr_t
  {
    template <typename Pattern>
    [[nodiscard]] constexpr auto like(Pattern pattern) const
    {
      return ::sqlpp::like(t, pattern);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto in(Exprs... exprs) const
    {
      return ::sqlpp::in(t, exprs...);
    }

    template <typename... Exprs>
    [[nodiscard]] constexpr auto not_in(Exprs... exprs) const
    {
      return ::sqlpp::not_in(t, exprs...);
    }

    [[nodiscard]] constexpr auto is_null() const
    {
      return ::sqlpp::is_null(t);
    }

    [[nodiscard]] constexpr auto is_not_null() const
    {
      return ::sqlpp::is_not_null(t);
    }

    [[nodiscard]] constexpr auto asc() const
    {
      return ::sqlpp::asc(t);
    }

    [[nodiscard]] constexpr auto desc() const
    {
      return ::sqlpp::desc(t);
    }

    template <typename Alias>
    [[nodiscard]] constexpr auto as(const Alias& alias) const
    {
      return ::sqlpp::as(t, alias);
    }

    T t;
  };

#warning implement type traits
#warning : also prevent alias to be wrapper in expression
  template <typename T>
  constexpr auto is_alias_v<expr_t<T>> = is_alias_v<T>;

  template <typename T>
  [[nodiscard]] auto expr(T t)
  {
    return expr_t{t};
  }
#warning implement to_sql_string

}  // namespace sqlpp
