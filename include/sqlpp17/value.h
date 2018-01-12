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

#include <string>
#include <string_view>

#include <sqlpp17/as_base.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename Expression>
  struct value_t : public as_base<value_t<Expression>>
  {
    value_t() = delete;
    constexpr value_t(Expression expression) : _expression(expression)
    {
    }
    value_t(const value_t&) = default;
    value_t(value_t&&) = default;
    value_t& operator=(const value_t&) = default;
    value_t& operator=(value_t&&) = default;
    ~value_t() = default;

    Expression _expression;
  };

  template <typename Expression>
  struct nodes_of<value_t<Expression>>
  {
    using type = type_vector<Expression>;
  };

  template <typename Expression>
  struct value_type_of<value_t<Expression>>
  {
    using type = Expression;
  };

  template <typename Expression, typename = std::enable_if_t<std::is_arithmetic_v<Expression>>>
  [[nodiscard]] constexpr auto value(Expression expression)
  {
    return value_t<Expression>{expression};
  }

  [[nodiscard]] auto value(std::string expression)
  {
    return value_t<std::string>{expression};
  }

  [[nodiscard]] constexpr auto value(std::string_view expression)
  {
    return value_t<std::string_view>{expression};
  }

  [[nodiscard]] constexpr auto value(const char* expression)
  {
    return value_t<const char*>{expression};
  }

  template <typename Context, typename Expression>
  [[nodiscard]] auto to_sql_string(Context& context, const value_t<Expression>& t)
  {
    return to_sql_string(context, t._expression);
  }
}  // namespace sqlpp
