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

#include <tuple>

#include <sqlpp17/core/clause_fwd.h>
#include <sqlpp17/core/result_row.h>
#include <sqlpp17/core/statement.h>
#include <sqlpp17/core/type_traits.h>
#include <sqlpp17/core/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename... Flags>
  struct select_flags_t
  {
    std::tuple<Flags...> _flags;
  };

  template <typename... Flags>
  struct nodes_of<select_flags_t<Flags...>>
  {
    using type = type_vector<Flags...>;
  };

  template <typename Table>
  constexpr auto clause_tag<select_flags_t<Table>> = ::std::string_view{"select_flags"};

  template <typename... Flags, typename Statement>
  class clause_base<select_flags_t<Flags...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<select_flags_t<Flags...>, OtherStatement>& s) : _flags(s._flags)
    {
    }

    clause_base(const select_flags_t<Flags...>& f) : _flags(f._flags)
    {
    }

    std::tuple<Flags...> _flags;
  };

  template <typename Context, typename... Flags, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<select_flags_t<Flags...>, Statement>& t)
  {
    return (std::string{} + ... + to_sql_string(context, std::get<Flags>(t._flags)));
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_flags_args_are_valid, "select flags() args must be valid select_flags");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_flags_args_are_unique, "select flags() args must have unique");

  template <typename... T>
  constexpr auto check_select_flags_arg(const T&...)
  {
    if constexpr (!(true && ... && is_select_flag_v<T>))
    {
      return failed<assert_select_flags_args_are_valid>{};
    }
    else if constexpr (type_set<T...>().size() != sizeof...(T))
    {
      return failed<assert_select_flags_args_are_unique>{};
    }
    else
      return succeeded{};
  }

  struct no_select_flags_t
  {
  };

  template <typename Statement>
  class clause_base<no_select_flags_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_select_flags_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename... Fields>
    [[nodiscard]] constexpr auto flags(Fields... flags) const
    {
      constexpr auto _check = check_select_flags_arg(flags...);
      if constexpr (_check)
      {
        return new_statement(*this, select_flags_t<Fields...>{std::tuple(flags...)});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }

    template <typename... Fields>
    [[nodiscard]] constexpr auto flags(std::tuple<Fields...> flags) const
    {
      constexpr auto _check = check_select_flags_arg(std::declval<Fields>()...);
      if constexpr (_check)
      {
        return new_statement(*this, select_flags_t<Fields...>{flags});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_select_flags_t, Statement>& t)
  {
    return std::string{};
  }

  template <typename... Fields>
  [[nodiscard]] constexpr auto select_flags(Fields&&... flags)
  {
    return statement<no_select_flags_t>{}.flags(std::forward<Fields>(flags)...);
  }
}  // namespace sqlpp
