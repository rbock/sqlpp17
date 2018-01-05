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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct offset
    {
    };
    struct order_by;
  }  // namespace clause

  template <typename Number>
  struct offset_t
  {
    Number _number;
  };

  template <typename Number>
  struct nodes_of<offset_t<Number>>
  {
    using type = type_vector<Number>;
  };

  template <typename Number>
  constexpr auto clause_tag<offset_t<Number>> = clause::offset{};

  template <typename Number, typename Statement>
  class clause_base<offset_t<Number>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<offset_t<Number>, OtherStatement>& s) : _number(s._number)
    {
    }

    clause_base(const offset_t<Number>& f) : _number(f._number)
    {
    }

    Number _number;
  };

  SQLPP_WRAPPED_STATIC_ASSERT(assert_offset_used_with_order_by, "offset must be used with order_by");

  template <typename Db, typename Number, typename... Clauses>
  constexpr auto check_clause_executable(const type_t<clause_base<offset_t<Number>, statement<Clauses...>>>& t)
  {
    constexpr auto _tag_set = type_set(clause_tag<Clauses>...);
    if constexpr (!_tag_set.template count<clause::order_by>())
    {
      return failed<assert_offset_used_with_order_by>{};
    }
    else
    {
      return succeeded{};
    }
  }

  template <typename Context, typename Number, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<offset_t<Number>, Statement>& t)
  {
    if (has_value(t._number))
      return std::string{};

    return std::string{" OFFSET "} + to_sql_string(context, get_value(t._number));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_offset_arg_is_integral_value, "offset() arg has to be an integral value");

  template <typename T>
  constexpr auto check_offset_arg()
  {
    if constexpr (!std::is_integral_v<T>)
    {
      return failed<assert_offset_arg_is_integral_value>{};
    }
    else
      return succeeded{};
  }

  struct no_offset_t
  {
  };

  template <typename Statement>
  class clause_base<no_offset_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_offset_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Value>
    [[nodiscard]] constexpr auto offset(Value value) const
    {
      constexpr auto check = check_offset_arg<remove_optional_t<Value>>();
      if constexpr (check)
      {
        return Statement::replace_clause(this, offset_t<Value>{value});
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_offset_t, Statement>&)
  {
    return std::string{};
  }

  template <typename Value>
  [[nodiscard]] constexpr auto offset(Value&& value)
  {
    return statement<no_offset_t>{}.offset(std::forward<Value>(value));
  }
}  // namespace sqlpp
