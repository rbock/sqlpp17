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

#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/unused.h>
#include <sqlpp17/optional.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct limit
    {
    };
  }

  template <typename Number>
  struct limit_t
  {
    Number _number;
  };

  template <typename Number>
  constexpr auto clause_tag<limit_t<Number>> = clause::limit{};

  template <typename Number, typename Statement>
  class clause_base<limit_t<Number>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<limit_t<Number>, OtherStatement>& s) : _number(s._number)
    {
    }

    clause_base(const limit_t<Number>& f) : _number(f._number)
    {
    }

    Number _number;
  };

#warning : Need to ensure order_by

  template <typename Context, typename Number, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<limit_t<Number>, Statement>& t)
  {
    if (detail::unused(t._number))
      return context;

    context << " LIMIT " << de_optionalize(t._number);

    return context;
  }
}
