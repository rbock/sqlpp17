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

#include <string_view>
#include <tables/TabDepartment.h>

#include <sqlpp17/parameter.h>

// Turning off static_assert for parameter<>.as()
namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_parameter_as_arg_is_name_tag_or_similar, T...> = true;
}  // namespace sqlpp

namespace
{
  template <typename Assert, typename T>
  auto test_bad_statement(const Assert&, const T&)
  {
    static_assert(::sqlpp::is_specific_bad_statement<Assert, T>());
  }

  template <typename Assert, typename T>
  auto test_good_statement(const Assert&, const T&)
  {
    static_assert(not::sqlpp::is_bad_statement<T>());
  }
}  // namespace

int main()
{
  test_bad_statement(::sqlpp::assert_parameter_as_arg_is_name_tag_or_similar{}, ::sqlpp::parameter<int>.as(17));

  test_good_statement(::sqlpp::assert_parameter_as_arg_is_name_tag_or_similar{},
                      ::sqlpp::parameter<int>.as(::test::tabDepartment.id));
}
