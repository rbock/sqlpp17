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

#include <sqlpp17/clause/from.h>
#include <sqlpp17/clause/group_by.h>
#include <sqlpp17/clause/having.h>
#include <sqlpp17/clause/limit.h>
#include <sqlpp17/clause/lock.h>
#include <sqlpp17/clause/offset.h>
#include <sqlpp17/clause/order_by.h>
#include <sqlpp17/clause/select_columns.h>
#include <sqlpp17/clause/select_flags.h>
#include <sqlpp17/clause/where.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/multi_column.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_select_args_are_flags_or_columns,
      "select() args must be a either select flags or selectable columns or a tuple of either kind");

  template <typename...>
  struct select_failure
  {
    using type = failed<assert_select_args_are_flags_or_columns>;
  };

  namespace clause
  {
    struct select
    {
    };
  }  // namespace clause

  struct select_t
  {
  };

  template <>
  constexpr auto clause_tag<select_t> = clause::select{};

  template <typename Statement>
  class clause_base<select_t, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<select_t, OtherStatement>&)
    {
    }

    clause_base() = default;
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<select_t, Statement>& t)
  {
    return std::string{"SELECT"};
  }

  // select with no args or an empty tuple yields a blank select statement

  [[nodiscard]] constexpr auto select()
  {
    return statement<select_t, no_select_flags_t, no_select_columns_t, no_from_t, no_where_t, no_group_by_t,
                     no_having_t, no_order_by_t, no_limit_t, no_offset_t, no_lock_t>{};
  }

  template <typename T = void>
  [[nodiscard]] constexpr auto select(std::tuple<>)
  {
    return ::sqlpp::bad_expression_t<typename select_failure<T>::type>{};
  }

  // select with at least one argument will either create flags or columns

  template <typename... Fs>
  [[nodiscard]] constexpr auto select(Fs... fs)
  {
    if constexpr ((true && ... && is_select_flag_v<Fs>))
    {
      return ::sqlpp::select().flags(fs...);
    }
    else if constexpr ((true && ... && is_selectable_v<Fs>))
    {
      return ::sqlpp::select().columns(fs...);
    }
    else
    {
      return ::sqlpp::bad_expression_t<typename select_failure<Fs...>::type>{};
    }
  }

  template <typename... Fs>
  [[nodiscard]] constexpr auto select(std::tuple<Fs...> fs)
  {
    if constexpr ((true && ... && is_select_flag_v<Fs>))
    {
      return ::sqlpp::select().flags(fs);
    }
    else if constexpr ((true && ... && is_selectable_v<Fs>))
    {
      return ::sqlpp::select().columns(fs);
    }
    else
    {
      return ::sqlpp::bad_expression_t<typename select_failure<Fs...>::type>{};
    }
  }

  template <typename... Fs>
  [[nodiscard]] constexpr auto select(multi_column_t<Fs...> mc)
  {
    return ::sqlpp::select().columns(mc);
  }
}  // namespace sqlpp
