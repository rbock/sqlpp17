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

#include <sqlpp17/member.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/table_alias.h>
#include <sqlpp17/join_functions.h>

namespace sqlpp
{
  template <typename Table, typename... ColumnSpecs>
  class table_t : public join_functions<Table>, public member_t<ColumnSpecs, column_t<Table, ColumnSpecs>>...
  {
    static_assert(sizeof...(ColumnSpecs), "at least one column required per table");

    auto& ref() const
    {
      return static_cast<const Table&>(this);
    }

  public:
    template <typename AliasProvider>
    auto as(const AliasProvider&) const
    {
      return table_alias_t<AliasProvider, Table>{ref()};
    }
  };

  template <typename Context, typename Table, typename... ColumnSpecs>
  struct interpreter_t<Context, table_t<Table, ColumnSpecs...>>
  {
    using T = table_t<Table, ColumnSpecs...>;

    static Context& _(const T&, Context& context)
    {
      context << name_of<T>::char_ptr();
      return context;
    }
  };
}

