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

#include <vector>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/statement.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename Table>
  struct from_t
  {
    Table _table;
  };

  template <typename Table>
  struct nodes_of<from_t<Table>>
  {
    using type = type_vector<Table>;
  };

  template <typename Table>
  constexpr auto clause_tag<from_t<Table>> = ::std::string_view{"from"};

  template <typename Table, typename Statement>
  class clause_base<from_t<Table>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<from_t<Table>, OtherStatement>& s) : _table(s._table)
    {
    }

    clause_base(const from_t<Table>& f) : _table(f._table)
    {
    }

    Table _table;
  };

  template <typename Context, typename Table, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<from_t<Table>, Statement>& t)
  {
    return std::string(" FROM ") + to_sql_string(context, t._table);
  }

  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_arg_is_not_conditionless_join,
                              "from() arg must not be a conditionless join, use .on() or .unconditionally()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_arg_is_table, "from() arg has to be a table or join");

  template <typename T>
  constexpr auto check_from_arg(const T&)
  {
    if constexpr (is_conditionless_join_v<T>)
    {
      return failed<assert_from_arg_is_not_conditionless_join>{};
    }
    else if constexpr (!is_table_v<T>)
    {
      return failed<assert_from_arg_is_table>{};
    }
    else
    {
      return succeeded{};
    }
  }

  struct no_from_t
  {
  };

  template <typename Statement>
  class clause_base<no_from_t, Statement>
  {
  public:
    template <typename OtherStatement>
    constexpr clause_base(const clause_base<no_from_t, OtherStatement>& s)
    {
    }

    constexpr clause_base() = default;

    template <typename Table>
    [[nodiscard]] constexpr auto from(Table t) const
    {
      constexpr auto _check = check_from_arg(t);
      if constexpr (_check)
      {
        return new_statement(*this, from_t<Table>{t});
      }
      else
      {
        return ::sqlpp::bad_expression_t{_check};
      }
    }
  };

  template <typename Context, typename Statement>
  [[nodiscard]] auto to_sql_string(Context& context, const clause_base<no_from_t, Statement>&)
  {
    return std::string{};
  }

  template <typename Table>
  [[nodiscard]] constexpr auto from(Table&& t)
  {
    return statement<no_from_t>{}.from(std::forward<Table>(t));
  }
}  // namespace sqlpp
