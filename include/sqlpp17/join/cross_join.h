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

#include <sqlpp17/join/join_functions.h>

namespace sqlpp
{
  template <typename Lhs, typename Rhs>
  class cross_join_t : public join_functions<cross_join_t<Lhs, Rhs>>
  {
  public:
    constexpr cross_join_t(Lhs lhs, Rhs rhs) : _lhs(lhs), _rhs(rhs)
    {
    }

    Lhs _lhs;
    Rhs _rhs;
  };

  template <typename Context, typename Lhs, typename Rhs>
  struct interpreter_t<Context, cross_join_t<Lhs, Rhs>>
  {
    using T = cross_join_t<Lhs, Rhs>;

    static Context& _(const T& t, Context& context)
    {
      interpret(t._lhs, context);
      context << " CROSS JOIN ";
      interpret(t._rhs, context);
      return context;
    }
  };

  template <typename Lhs, typename Rhs>
  constexpr auto is_join_v<cross_join_t<Lhs, Rhs>> = true;

  template <typename Lhs, typename Rhs>
  constexpr auto is_table_v<cross_join_t<Lhs, Rhs>> = true;

  template <typename Lhs, typename Rhs>
  constexpr auto provided_tables_of_v<cross_join_t<Lhs, Rhs>> = provided_tables_of_v<Lhs> | provided_tables_of_v<Rhs>;
}

