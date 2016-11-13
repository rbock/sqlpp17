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
#include <sqlpp17/interpretable.h>
#include <sqlpp17/interpret.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_from_add_arg_is_not_conditionless_dynamic_join,
      "from.add() arg must not be a conditionless dynamic join, use on() or .unconditionally()");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_add_arg_is_dynamic_join, "from::add() arg must be a dynamic join");
  SQLPP_WRAPPED_STATIC_ASSERT(assert_from_add_provides_no_known_table_names,
                              "from::add() arg must not add table names already used in from()");

  namespace detail
  {
#warning need to implement from::add
    template <typename Table, typename T>
    constexpr auto check_from_add(const T&)
    {
      if
        constexpr(is_conditionless_dynamic_join<T>)
        {
          return failed<assert_from_add_arg_is_not_conditionless_dynamic_join>{};
        }
      else if
        constexpr(!is_dynamic_join<T>)
        {
          return failed<assert_from_add_arg_is_dynamic_join>{};
        }
      else if
        constexpr(!provided_table_names_of<Table>.template count<name_of<T>>())
        {
          return failed<assert_from_add_provides_no_known_table_names>{};
        }
      else
        return succeeded{};
    }
  }

  template <typename Connection, typename Table>
  class from_data
  {
    Table _t;
    std::vector<interpretable_t<Connection>> _dynamic_joins;

    template <typename Join>
    auto add_impl(Join join)
    {
    }

  public:
    void add();

    from_data(const from_data<void, Table>& data) : _t(data._t)
    {
    }
  };

  template <typename Table>
  struct from_data<void, Table>
  {
    Table _t;
  };

  template <typename Table>
  struct from_t
  {
    from_data<void, Table> _data;
  };

  template <typename Table, typename Connection, typename Statement>
  class clause_base<from_t<Table>, Connection, Statement>
  {
  public:
    clause_base(const from_t<Table>& f) : from(f._data)
    {
    }

    from_data<Connection, Table> from;
  };

  template <typename Context, typename Table, typename Connection, typename Statement>
  class interpreter_t<Context, clause_base<from_t<Table>, Connection, Statement>>
  {
    using T = clause_base<from_t<Table>, Connection, Statement>;

    static Context& _(const T& t, Context& context)
    {
      context << " FROM ";
      interpret(t._table, context);
      if
        constexpr(!std::is_same<Connection, void>::value)
        {
          for (const auto& dynamic_join : t._dynamic_joins)
          {
            interpret(dynamic_join, context);
          }
        }
      return context;
    }
  };
}
