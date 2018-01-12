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

#include <sqlpp17/name_tag.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/parameter.h>

#include <connections/mock_db.h>
#include <serialize/assert_equality.h>

using ::sqlpp::test::assert_equality;
using ::sqlpp::test::mock_context_t;

SQLPP_CREATE_NAME_TAG(foo);
SQLPP_CREATE_NAME_TAG(bar);

namespace test
{
  struct count_context_t
  {
    int parameter_index = 0;
  };
}  // namespace test

namespace sqlpp
{
  template <typename ValueType, typename NameTag>
  [[nodiscard]] auto to_sql_string(::test::count_context_t& context, const parameter_t<ValueType, NameTag>& t)
  {
    return "$" + std::to_string(context.parameter_index++);
  }
}  // namespace sqlpp

int main()
{
  try
  {
    assert_equality("? = ?",
                    to_sql_string_c(mock_context_t{}, ::sqlpp::parameter<int>(foo) == ::sqlpp::parameter<int>(bar)));
    assert_equality("$0 < $1", to_sql_string_c(test::count_context_t{},
                                               ::sqlpp::parameter<int>(foo) < ::sqlpp::parameter<int>(bar)));
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return -1;
  }
}
