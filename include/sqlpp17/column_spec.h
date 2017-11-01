#pragma once

/*
Copyright (c) 2017, Roland Bock
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

#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename Alias, typename ValueType, tag::type Tags>
  struct column_spec
  {
    using _alias_t = Alias;
    using value_type = ValueType;
    static constexpr auto tags = Tags;
  };

  template <typename Alias, typename ValueType, tag::type Tags>
  struct value_type_of<column_spec<Alias, ValueType, Tags>>
  {
    using type = ValueType;
  };

  template <typename LeftAlias,
            typename LeftValueType,
            tag::type LeftTags,
            typename RightAlias,
            typename RightValueType,
            tag::type RightTags>
  constexpr auto column_specs_are_compatible_v<column_spec<LeftAlias, LeftValueType, LeftTags>,
                                               column_spec<RightAlias, RightValueType, RightTags>> =
      (char_sequence_of_v<LeftAlias> == char_sequence_of_v<RightAlias> &&
       std::is_same_v<LeftValueType, RightValueType>);

  template <typename Statement, typename OptColumn>
  struct make_column_spec
  {
    using _opt_column_t = OptColumn;
    using _column_t = remove_optional_t<OptColumn>;
    using _alias_t = typename _column_t::_alias_t;
    using _value_t = value_type_of_t<_column_t>;

#warning : being in outer tables might be relevant -> optional
    // static constexpr auto _outer_tables = outer_tables_of_v<Statement>;
    using type = column_spec<_alias_t, _value_t, 0>;
  };

  template <typename Statement, typename OptColumn>
  using make_column_spec_t = typename make_column_spec<Statement, OptColumn>::type;
}  // namespace sqlpp
