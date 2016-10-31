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

#include <sqlpp17/column.h>
#include <sqlpp17/interpreter.h>
#include <sqlpp17/join_functions.h>
#include <sqlpp17/member.h>

namespace sqlpp
{
  template <typename AliasProvider, typename Table, typename... ColumnSpecs>
  struct table_alias_t : public join_functions<table_alias_t<AliasProvider, Table, ColumnSpecs...>>,
                         public member_t<ColumnSpecs, column_t<AliasProvider, ColumnSpecs>>...
  {
    Table _table;
  };

  template <typename Context, typename AliasProvider, typename Table, typename... ColumnSpecs>
  struct interpreter_t<Context, table_alias_t<AliasProvider, Table, ColumnSpecs...>>
  {
    using T = table_alias_t<AliasProvider, Table, ColumnSpecs...>;

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
}

