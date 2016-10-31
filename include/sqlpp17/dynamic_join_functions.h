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

#include <sqlpp11/join_types.h>

namespace sqlpp
{
  template <typename JoinType, typename Rhs>
  class dynamic_pre_join_t;

  template <typename Rhs>
  struct dynamic_cross_join_t;

  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_pre_join_table, "argument of dynamic_join() has to be a table");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_pre_join_no_join, "argument of dynamic_join() must not be a join");
  SQLPP_PORTABLE_STATIC_ASSERT(assert_dynamic_pre_join_no_table_dependencies,
                               "dynamically joined tables must not depend on other tables");

  namespace detail
  {
    template <typename Rhs>
    constexpr auto check_dynamic_pre_join(const Rhs&)
    {
      if
        constexpr(!is_table<Rhs>)
        {
          return failed<assert_dynamic_pre_join_table>{};
        }
      else if
        constexpr(is_join_t<Rhs>)
        {
          return failed<assert_dynamic_pre_join_no_join>{};
        }
      else if
        constexpr(!required_tables_of<Rhs>::empty())
        {
          return failed<assert_dynamic_pre_join_no_table_dependencies>{};
        }
      else
        return succeeded{};
    }

    template <typename JoinType, typename Rhs>
    auto dynamic_join_impl(Rhs rhs)
    {
      constexpr auto check = check_dynamic_pre_join(rhs);
      if
        constexpr(check)
        {
          return dynamic_pre_join_t<JoinType, Rhs>{rhs};
        }
      else
      {
        return check;
      }
    }
  }

  template <typename Rhs>
  auto dynamic_join(Rhs rhs) -> make_return_type<decltype(dynamic_join_impl<inner_join>(rhs))>
  {
    return dynamic_join_impl<inner_join>(rhs);
  }

  template <typename Rhs>
  auto dynamic_inner_join(Rhs rhs)
  {
    return inner_join(rhs);
  }

  template <typename Rhs>
  auto dynamic_left_outer_join(Rhs rhs) -> make_return_type<decltype(dynamic_join_impl<left_outer_join>(rhs))>
  {
    return dynamic_join_impl<left_outer_join>(rhs);
  }

  template <typename Rhs>
  auto dynamic_right_outer_join(Rhs rhs) -> make_return_type<decltype(dynamic_join_impl<right_outer_join>(rhs))>
  {
    return dynamic_join_impl<right_outer_join>(rhs);
  }

  template <typename Rhs>
  auto dynamic_outer_join(Rhs rhs) -> make_return_type<decltype(dynamic_join_impl<outer_join>(rhs))>
  {
    return dynamic_join_impl<outer_join>(rhs);
  }

  namespace detail
  {
    template <typename Rhs>
    auto dynamic_cross_join_impl(Rhs rhs)
    {
      constexpr auto check = dynamic_check_pre_join(rhs);
      if
        constexpr(check)
        {
          return dynamic_cross_join_t<Rhs>{rhs};
        }
      else
      {
        return check;
      }
    }
  }

  template <typename Rhs>
  auto dynamic_cross_join(Rhs rhs) -> make_return_type<decltype(dynamic_cross_join_impl(rhs))>
  {
    return dynamic_cross_join_impl(rhs);
  }
}

