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
#include <tuple>
#include <sqlpp17/detail/separator.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  namespace clause
  {
    struct selected_fields
    {
    };
  }

  template <typename... Fields>
  struct selected_fields_t
  {
    std::tuple<Fields...> _fields;
  };

  template <typename... Fields>
  constexpr auto is_result_clause_v<selected_fields_t<Fields...>> = true;

  template <typename Table>
  constexpr auto clause_tag<selected_fields_t<Table>> = clause::selected_fields{};

  template <typename... Fields, typename Statement>
  class clause_base<selected_fields_t<Fields...>, Statement>
  {
  public:
    template <typename OtherStatement>
    clause_base(const clause_base<selected_fields_t<Fields...>, OtherStatement>& s) : _fields(s._fields)
    {
    }

    clause_base(const selected_fields_t<Fields...>& f) : _fields(f._fields)
    {
    }

    std::tuple<Fields...> _fields;
  };

  template <typename... Fields, typename Statement>
  class result_base<selected_fields_t<Fields...>, Statement>
  {
  public:
    void run() const
    {
    }
#warning : Need to prepare some result functionality
  };

#warning : The dynamic vector variant is missing

  template <typename Context, typename... Fields, typename Statement>
  decltype(auto) operator<<(Context& context, const clause_base<selected_fields_t<Fields...>, Statement>& t)
  {
    auto separate = detail::separator<Context>{context, ", "};
    context << ' ';
    return (context << ... << separate(std::get<Fields>(t._fields)));
  }
}
