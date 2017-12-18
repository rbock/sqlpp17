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
  template <typename Alias, typename ValueType>
  struct column_spec
  {
    using _alias_t = Alias;
    using value_type = ValueType;
#warning : Pseudo-tables need to be read-only
    static constexpr auto tags = tag::none;
  };

  template <typename Alias, typename ValueType>
  struct value_type_of<column_spec<Alias, ValueType>>
  {
    using type = ValueType;
  };

  template <typename Alias, typename ValueType>
  static constexpr auto char_sequence_of_v<column_spec<Alias, ValueType>> = char_sequence_of_v<Alias>;

  template <typename Statement, typename Column>
  struct make_column_spec
  {
    using _base_column = remove_optional_t<Column>;
    using _value_t = value_type_of_t<_base_column>;
    using _base_value_t = remove_optional_t<_value_t>;

    using _alias_t = typename _base_column::_alias_t;

    static constexpr auto _is_optional =
        (is_optional_v<Column> or is_optional_v<_value_t> or can_be_null_v<_base_column> or
         optional_columns_of_v<Statement>.template count<_base_column>());

    using _spec_value_t = std::conditional_t<_is_optional, add_optional_t<_base_value_t>, _base_value_t>;

    using type = column_spec<_alias_t, _spec_value_t>;
  };

  template <typename Statement, typename Column>
  using make_column_spec_t = typename make_column_spec<Statement, Column>::type;

}  // namespace sqlpp
