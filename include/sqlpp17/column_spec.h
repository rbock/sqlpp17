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
  constexpr auto value_type_of_v<column_spec<Alias, ValueType, Tags>> = ValueType{};

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto can_be_null_v<column_spec<Alias, ValueType, Tags>> = !!(Tags & tag::can_be_null);

  template <typename Alias, typename ValueType, tag::type Tags>
  constexpr auto null_is_trivial_value_v<column_spec<Alias, ValueType, Tags>> = !!(Tags & tag::null_is_trivial_value);

  template <typename LeftAlias,
            typename LeftValueType,
            tag::type LeftTags,
            typename RightAlias,
            typename RightValueType,
            tag::type RightTags>
  constexpr auto column_specs_are_compatible_v<column_spec<LeftAlias, LeftValueType, LeftTags>,
                                               column_spec<RightAlias, RightValueType, RightTags>> =
      (std::is_same_v<make_char_sequence_t<LeftAlias::name>, make_char_sequence_t<RightAlias::name>> &&
       std::is_same_v<LeftValueType, RightValueType>);

  template <typename Statement, typename OptColumn>
  struct make_column_spec
  {
    using _opt_column_t = OptColumn;
    using _column_t = remove_optional_t<OptColumn>;
    using _alias_t = typename _column_t::_alias_t;
    using _value_t = value_type_of_t<_column_t>;

    static constexpr auto _outer_tables = outer_tables_of_v<Statement>;
    static constexpr auto _column_can_be_null = can_be_null_v<_column_t>;
    static constexpr auto _column_is_optional = is_optional_v<_opt_column_t>;
    static constexpr auto _column_is_in_outer_table = can_be_null_v<_column_t>;
    static constexpr auto _can_be_null_tag =
        tag_if_v<tag::can_be_null, _column_can_be_null | _column_is_optional | _column_is_in_outer_table>;

    static constexpr auto _null_is_trivial_value = null_is_trivial_value_v<_opt_column_t>;

    static constexpr auto _null_is_trivial_value_tag = tag_if_v<tag::null_is_trivial_value, _null_is_trivial_value>;
    using type = column_spec<_alias_t, _value_t, _can_be_null_tag | _null_is_trivial_value_tag>;
  };

  template <typename Statement, typename OptColumn>
  using make_column_spec_t = typename make_column_spec<Statement, OptColumn>::type;
}
