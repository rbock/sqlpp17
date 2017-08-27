#pragma once

/*
Copyright (c) 2016-2017, Roland Bock
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

#include <sqlpp17/bad_statement.h>
#include <sqlpp17/join/join_types.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename Lhs, typename JoinType, typename Rhs>
  class conditionless_join_t;

  SQLPP_WRAPPED_STATIC_ASSERT(assert_conditionless_join_lhs_table,
                              "lhs argument of join() has to be a table or a join");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_conditionless_join_rhs_table, "rhs argument of join() has to be a table");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_conditionless_join_unique_names, "joined table names have to be unique");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_conditionless_join_no_table_dependencies,
                              "joined tables must not depend on other tables");

  namespace detail
  {
    template <typename Lhs, typename Rhs>
    constexpr auto check_conditionless_join(const Lhs&, const Rhs&)
    {
      if constexpr (!is_table_v<Lhs>)
      {
        return failed<assert_conditionless_join_lhs_table>{};
      }
      else if constexpr (!is_table_v<remove_optional_t<Rhs>>)
      {
        return failed<assert_conditionless_join_rhs_table>{};
      }
      else if constexpr (!provided_table_names_of_v<Lhs>.is_disjoint_from(
                             provided_table_names_of_v<remove_optional_t<Rhs>>))
      {
        return failed<assert_conditionless_join_unique_names>{};
      }
      else if constexpr (!required_tables_of_v<Lhs>.empty() || !required_tables_of_v<remove_optional_t<Rhs>>.empty())
      {
        return failed<assert_conditionless_join_no_table_dependencies>{};
      }
      else
        return succeeded{};
    }

    template <typename JoinType, typename Lhs, typename Rhs>
    constexpr auto join_impl(Lhs lhs, Rhs rhs)
    {
      constexpr auto check = check_conditionless_join(lhs, rhs);
      if constexpr (check)
      {
        return conditionless_join_t{lhs, JoinType{}, rhs};
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto inner_join(Lhs lhs, Rhs rhs)
  {
    return detail::join_impl<inner_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto join(Lhs lhs, Rhs rhs)
  {
    return inner_join(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto left_outer_join(Lhs lhs, Rhs rhs)
  {
    return detail::join_impl<left_outer_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto right_outer_join(Lhs lhs, Rhs rhs)
  {
    return detail::join_impl<right_outer_join_t>(lhs, rhs);
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto outer_join(Lhs lhs, Rhs rhs)
  {
    return detail::join_impl<outer_join_t>(lhs, rhs);
  }

  namespace detail
  {
    template <typename Lhs, typename Rhs>
    constexpr auto cross_join_impl(Lhs lhs, Rhs rhs)
    {
      constexpr auto check = check_conditionless_join(lhs, rhs);
      if constexpr (check)
      {
        return detail::join_impl<cross_join_t>(lhs, rhs).unconditionally();
      }
      else
      {
        return ::sqlpp::bad_statement_t{check};
      }
    }
  }

  template <typename Lhs, typename Rhs>
  [[nodiscard]] constexpr auto cross_join(Lhs lhs, Rhs rhs)
  {
    return detail::cross_join_impl(lhs, rhs);
  }

  template <typename Derived>
  class join_functions
  {
    constexpr decltype(auto) ref() const
    {
      return static_cast<const Derived&>(*this);
    }

  public:
    template <typename T>
    [[nodiscard]] constexpr auto join(T t) const
    {
      return ::sqlpp::join(ref(), t);
    }

    template <typename T>
    [[nodiscard]] constexpr auto inner_join(T t) const
    {
      return ::sqlpp::inner_join(ref(), t);
    }

    template <typename T>
    [[nodiscard]] constexpr auto left_outer_join(T t) const
    {
      return ::sqlpp::left_outer_join(ref(), t);
    }

    template <typename T>
    [[nodiscard]] constexpr auto right_outer_join(T t) const
    {
      return ::sqlpp::right_outer_join(ref(), t);
    }

    template <typename T>
    [[nodiscard]] constexpr auto outer_join(T t) const
    {
      return ::sqlpp::outer_join(ref(), t);
    }

    template <typename T>
    [[nodiscard]] constexpr auto cross_join(T t) const
    {
      return ::sqlpp::cross_join(ref(), t);
    }
  };
}
