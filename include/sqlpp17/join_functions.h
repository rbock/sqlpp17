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

#include <sqlpp17/join_types.h>
#include <sqlpp17/make_return_type.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename JoinType, typename Lhs, typename Rhs>
  class pre_join_t;

  template <typename Lhs, typename Rhs>
  struct cross_join_t;

  SQLPP_WRAPPED_STATIC_ASSERT(assert_pre_join_lhs_table, "lhs argument of join() has to be a table or a join");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_pre_join_rhs_table, "rhs argument of join() has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_pre_join_rhs_no_join, "rhs argument of join() must not be a join");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_pre_join_unique_names, "joined table names have to be unique");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_pre_join_no_table_dependencies, "joined tables must not depend on other tables");

  namespace detail
  {
    template <typename Lhs, typename Rhs>
    constexpr auto check_pre_join(const Lhs&, const Rhs&)
    {
      if
        constexpr(!is_table<Lhs>)
        {
          return failed<assert_pre_join_lhs_table>{};
        }
      else if
        constexpr(!is_table<Rhs>)
        {
          return failed<assert_pre_join_rhs_table>{};
        }
      else if
        constexpr(is_join<Rhs>)
        {
          return failed<assert_pre_join_rhs_no_join>{};
        }
      else if
        constexpr(!provided_table_names_of<Lhs>.is_disjoint_from(provided_table_names_of<Rhs>))
        {
          return failed<assert_pre_join_unique_names>{};
        }
      else if
        constexpr(!required_tables_of<Lhs>.empty() || !required_tables_of<Rhs>.empty())
        {
          return failed<assert_pre_join_no_table_dependencies>{};
        }
      else
        return succeeded{};
    }

    template <typename JoinType, typename Lhs, typename Rhs>
    auto join_impl(Lhs lhs, Rhs rhs)
    {
      constexpr auto check = check_pre_join(lhs, rhs);
      if
        constexpr(check)
        {
          return pre_join_t<JoinType, Lhs, Rhs>{lhs, rhs};
        }
      else
      {
        return check;
      }
    }
  }

  template <typename Lhs, typename Rhs>
  auto join(Lhs lhs, Rhs rhs) -> make_return_type<decltype(detail::join_impl<inner_join_t>(lhs, rhs))>
  {
    return detail::join_impl<inner_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  auto inner_join(Lhs lhs, Rhs rhs)
  {
    return inner_join(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  auto left_outer_join(Lhs lhs, Rhs rhs) -> make_return_type<decltype(detail::join_impl<left_outer_join_t>(lhs, rhs))>
  {
    return detail::join_impl<left_outer_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  auto right_outer_join(Lhs lhs, Rhs rhs) -> make_return_type<decltype(detail::join_impl<right_outer_join_t>(lhs, rhs))>
  {
    return detail::join_impl<right_outer_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  auto outer_join(Lhs lhs, Rhs rhs) -> make_return_type<decltype(detail::join_impl<outer_join_t>(lhs, rhs))>
  {
    return detail::join_impl<outer_join_t>(lhs, rhs);
  }

  namespace detail
  {
    template <typename Lhs, typename Rhs>
    auto cross_join_impl(Lhs lhs, Rhs rhs)
    {
      constexpr auto check = check_pre_join(rhs);
      if
        constexpr(check)
        {
          return cross_join_t<Lhs, Rhs>{lhs, rhs};
        }
      else
      {
        return check;
      }
    }
  }

  template <typename Lhs, typename Rhs>
  auto cross_join(Lhs lhs, Rhs rhs) -> make_return_type<decltype(cross_join_impl(lhs, rhs))>
  {
    return cross_join_impl(lhs, rhs);
  }

  template <typename Derived>
  class join_functions
  {
    auto& ref() const
    {
      return static_cast<const Derived&>(this);
    }

  public:
    template <typename T>
    auto join(T t) const
    {
      return ::sqlpp::join(ref(), t);
    }

    template <typename T>
    auto inner_join(T t) const
    {
      return ::sqlpp::inner_join(ref(), t);
    }

    template <typename T>
    auto left_outer_join(T t) const
    {
      return ::sqlpp::left_outer_join(ref(), t);
    }

    template <typename T>
    auto right_outer_join(T t) const
    {
      return ::sqlpp::right_outer_join(ref(), t);
    }

    template <typename T>
    auto outer_join(T t) const
    {
      return ::sqlpp::outer_join(ref(), t);
    }

    template <typename T>
    auto cross_join(T t) const
    {
      return ::sqlpp::cross_join(ref(), t);
    }
  };
}

