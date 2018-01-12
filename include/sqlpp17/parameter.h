#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <sqlpp17/as_base.h>
#include <sqlpp17/bad_expression.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename ValueType, typename NameTag>
  struct parameter_t : public as_base<parameter_t<ValueType, NameTag>>
  {
  };

  template <typename ValueType, typename NameTag>
  struct value_type_of<parameter_t<ValueType, NameTag>>
  {
    using type = ValueType;
  };

  template <typename ValueType, typename NameTag>
  struct name_tag_of<parameter_t<ValueType, NameTag>>
  {
    using type = NameTag;
  };

  template <typename ValueType, typename NameTag>
  [[nodiscard]] constexpr auto parameters_of([[maybe_unused]] type_t<parameter_t<ValueType, NameTag>>)
  {
    return type_vector<NameTag>{};
  }

  SQLPP_WRAPPED_STATIC_ASSERT(
      assert_parameter_as_arg_is_name_tag_or_similar,
      "parameter() arg must be a named expression (e.g. column, table), or a column/table spec, or a name tag");

  template <typename Tag>
  constexpr auto check_parameter_args()
  {
    if constexpr (std::is_same_v<name_tag_of_t<Tag>, none_t>)
    {
      return failed<assert_parameter_as_arg_is_name_tag_or_similar>{};
    }
    else
      return succeeded{};
  }

  template <typename ValueType>
  struct unnamed_parameter_t
  {
    template <typename NamedTypeOrTag>
    [[nodiscard]] constexpr auto operator()([[maybe_unused]] NamedTypeOrTag) const
    {
      if constexpr (constexpr auto check = check_parameter_args<NamedTypeOrTag>(); check)
      {
        return parameter_t<ValueType, name_tag_of_t<NamedTypeOrTag>>{};
      }
      else
      {
        return ::sqlpp::bad_expression_t{check};
      }
    }
  };

  template <typename ValueType>
  static constexpr auto parameter = unnamed_parameter_t<ValueType>{};

  template <typename Context, typename ValueType, typename NameTag>
  [[nodiscard]] auto to_sql_string(Context& context, const parameter_t<ValueType, NameTag>& t)
  {
    return std::string{"?"};
  }

}  // namespace sqlpp
