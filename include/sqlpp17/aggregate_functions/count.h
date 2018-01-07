#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <sqlpp17/type_traits.h>

#include <sqlpp17/aggregate.h>
#include <sqlpp17/bad_expression.h>
#include <sqlpp17/flags.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_count_arg_is_expression, "count() arg must be a value expression");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_count_arg_is_not_alias, "count() arg must not be an alias");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_count_arg_is_not_aggregate, "count() arg must not be an aggregate");

  template <typename Expr>
  constexpr auto check_count_args()
  {
    if constexpr (not is_expression_v<Expr>)
    {
      return failed<assert_count_arg_is_expression>{};
    }
    else if constexpr (is_alias_v<Expr>)
    {
      return failed<assert_count_arg_is_not_alias>{};
    }
    else if constexpr (::sqlpp::is_aggregate_v<Expr>)
    {
      return failed<assert_count_arg_is_not_aggregate>{};
    }
    else
      return succeeded{};
  }

  template <typename Flag>
  struct count_t
  {
    static constexpr auto name = std::string_view{"COUNT"};
    using flag_type = Flag;
    using value_type = int64_t;
  };

  template <typename Expr>
  [[nodiscard]] constexpr auto count(Expr expr)
  {
    if constexpr (constexpr auto check = check_count_args<Expr>(); check)
    {
      return aggregate_t<count_t<no_flag_t>, Expr>{expr};
    }
    else
    {
      return ::sqlpp::bad_expression_t{check};
    }
  }

  template <typename Expr>
  [[nodiscard]] constexpr auto count_distinct(Expr expr)
  {
    if constexpr (constexpr auto check = check_count_args<Expr>(); check)
    {
      return aggregate_t<count_t<distinct_t>, Expr>{expr};
    }
    else
    {
      return ::sqlpp::bad_expression_t{check};
    }
  }

}  // namespace sqlpp
