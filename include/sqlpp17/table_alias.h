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

#include <sqlpp17/char_sequence.h>
#include <sqlpp17/column.h>
#include <sqlpp17/interpreter.h>
#include <sqlpp17/join.h>
#include <sqlpp17/member.h>

namespace sqlpp
{
  template <typename Table, typename Alias, typename... ColumnSpecs>
  struct table_alias_t : public join_functions<table_alias_t<Table, Alias, ColumnSpecs...>>,
                         public member_t<ColumnSpecs, column_t<Alias, ColumnSpecs>>...
  {
    Table _table;
  };

  template <typename Context, typename Table, typename Alias, typename... ColumnSpecs>
  struct interpreter_t<Context, table_alias_t<Table, Alias, ColumnSpecs...>>
  {
    using T = table_alias_t<Table, Alias, ColumnSpecs...>;

    static Context& _(const T& t, Context& context)
    {
      if
        constexpr(requires_braces<Table>) context << "(";
      serialize(t._table, context);
      if
        constexpr(requires_braces<Table>) context << ")";
      context << " AS " << name_of<T>::char_ptr();
      return context;
    }
  };

  template <typename Table, typename Alias, typename... ColumnSpecs>
  constexpr auto is_table_v<table_alias_t<Table, Alias, ColumnSpecs...>> = true;

  template <typename Table, typename Alias, typename... ColumnSpecs>
  constexpr auto provided_tables_of_v<table_alias_t<Table, Alias, ColumnSpecs...>> =
      type_set<table_alias_t<Table, Alias, ColumnSpecs...>>();

  template <typename Table, typename Alias, typename... ColumnSpecs>
  struct char_sequence_of_impl<table_alias_t<Table, Alias, ColumnSpecs...>>
  {
    using type = make_char_sequence<Alias::_alias_t::name>;
  };
}

