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
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct union_
    {
    };
  }

  template <typename LeftSelect, typename RightSelect>
  struct union_t
  {
    LeftSelect _left;
    RightSelect _right;
  };

  template <typename LeftSelect, typename RightSelect>
  constexpr auto clause_tag<union_t<LeftSelect, RightSelect>> = clause::union_{};

  template <typename LeftSelect, typename RightSelect, typename Statement>
  class clause_base<union_t<LeftSelect, RightSelect>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<union_t<LeftSelect, RightSelect>, OtherStatement>& s)
        : _left(s._left), _right(s._right)
    {
    }

    clause_base(const union_t<LeftSelect, RightSelect>& f) : _left(f._left), _right(f._right)
    {
    }

    LeftSelect _left;
    RightSelect _right;
  };

  template <typename Context, typename LeftSelect, typename RightSelect, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<union_t<LeftSelect, RightSelect>, Statement>& t)
  {
    return context << embrace(t._left) << " UNION " << embrace(t._right);
  }
}