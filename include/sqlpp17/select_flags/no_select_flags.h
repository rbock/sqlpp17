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
#include <sqlpp17/all.h>
#include <sqlpp17/select_flags/select_flags.h>
#include <sqlpp17/statement.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_flags_args_are_valid, "select flags() args must be valid select_flags");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_select_flags_args_are_unique, "select flags() args must have unique");

  template <typename... T>
  constexpr auto check_select_flags_arg(const T&...)
  {
    if
      constexpr(!all<is_select_flag_v<T>...>)
      {
        return failed<assert_select_flags_args_are_valid>{};
      }
    else if
      constexpr(type_set<T...>().size() != sizeof...(T))
      {
        return failed<assert_select_flags_args_are_unique>{};
      }
    else
      return succeeded{};
  }

  struct no_select_flags_t
  {
  };

  template <>
  constexpr auto clause_tag<no_select_flags_t> = clause::select_flags{};

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
      constexpr auto check = check_select_flags_arg(flags...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_select_flags_t>(
              select_flags_t<Fields...>{std::make_tuple(flags...)});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }

    template <typename... Fields>
    [[nodiscard]] constexpr auto flags(std::tuple<Fields...> flags) const
    {
      constexpr auto check = check_select_flags_arg(std::declval<Fields>()...);
      if
        constexpr(check)
        {
          return Statement::of(this).template replace_clause<no_select_flags_t>(select_flags_t<Fields...>{flags});
        }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  };

  template <typename Context, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<no_select_flags_t, Statement>& t)
  {
    return context;
  }

  template <typename... Fields>
  [[nodiscard]] constexpr auto select_flags(Fields&&... flags)
  {
    return statement<no_select_flags_t>{}.flags(std::forward<Fields>(flags)...);
  }
}

