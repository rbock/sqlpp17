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

#include <sqlpp17/char_sequence.h>
#include <sqlpp17/to_sql_string.h>
#include <sqlpp17/type_traits.h>

namespace sqlpp
{
  template <typename Expression, typename NameTag>
  struct alias_t
  {
    using _alias_t = NameTag;
    Expression _expression;
  };

  template <typename Expression, typename NameTag>
  struct value_type_of<alias_t<Expression, NameTag>>
  {
    using type = value_type_of_t<Expression>;
  };

  template <typename Expression, typename NameTag>
  struct name_tag_of<alias_t<Expression, NameTag>>
  {
    using type = NameTag;
  };

  template <typename Expression, typename NameTag>
  constexpr auto is_alias_v<alias_t<Expression, NameTag>> = true;

  template <typename Expression, typename NameTag>
  constexpr auto is_selectable_v<alias_t<Expression, NameTag>> = is_expression_v<alias_t<Expression, NameTag>>;

  template <typename DbConnection, typename Expression, typename NameTag>
  [[nodiscard]] auto to_sql_string(const DbConnection& connection, const alias_t<Expression, NameTag>& t)
  {
    return to_sql_string(connection, t._expression) + " AS " + to_sql_name(connection, t);
  }
}  // namespace sqlpp
