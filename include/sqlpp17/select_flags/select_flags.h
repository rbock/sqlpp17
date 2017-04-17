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
#include <vector>

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/list_printer.h>
#include <sqlpp17/result_row.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct select_flags
    {
    };
  }

  template <typename... Flags>
  struct select_flags_t
  {
    std::tuple<Flags...> _flags;
  };

  template <typename Table>
  constexpr auto clause_tag<select_flags_t<Table>> = clause::select_flags{};

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
  decltype(auto) operator<<(Context& context, const clause_base<select_flags_t<Flags...>, Statement>& t)
  {
    auto separate = detail::list_printer<Context>{context, " "};
    context << ' ';
    (..., print(std::get<Flags>(t._flags)));
    return context;
  }
}
