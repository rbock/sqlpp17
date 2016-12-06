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

#include <utility>

namespace sqlpp
{
  namespace detail
  {
    // Statements inherit from clauses. They can be used as constructor
    // arguments for clauses.
    // The statement_constructor_arg inherits from fragments
    // (statements or other clause constructor arguments).
    //
    // Thus, if initialized with the right fragments, the
    // statement_constructor_arg can be used to construct all clauses
    // in the new statement.
    template <typename... Fragments>
    struct statement_constructor_arg : Fragments...
    {
      template <typename... Ts>
      constexpr statement_constructor_arg(Ts&&... ts) : Fragments{std::forward<Ts>(ts)}...
      {
      }
    };

    template <typename... Fragments>
    constexpr auto make_constructor_arg(Fragments&&... fragments)
    {
      return statement_constructor_arg<std::decay_t<Fragments>...>{std::forward<Fragments>(fragments)...};
    }
  }
}

