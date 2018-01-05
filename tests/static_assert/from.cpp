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

#include <string>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

#include <sqlpp17/clause/from.h>

#include <static_assert/assert_bad_expression.h>

using ::sqlpp::test::assert_bad_expression;
using ::sqlpp::test::assert_good_expression;

// Turning off static_assert for from()
namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_from_arg_is_table, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_from_arg_is_not_conditionless_join, T...> = true;
}  // namespace sqlpp

int main()
{
  constexpr auto s = sqlpp::statement<sqlpp::no_from_t>{};

  // constexpr tests
  assert_bad_expression(sqlpp::assert_from_arg_is_table{}, s.from(1));

  assert_bad_expression(sqlpp::assert_from_arg_is_not_conditionless_join{},
                        s.from(test::tabPerson.join(test::tabDepartment)));

  assert_bad_expression(sqlpp::assert_from_arg_is_table{}, sqlpp::from(1));

  // non-constexpr tests
  assert_bad_expression(sqlpp::assert_from_arg_is_table{}, sqlpp::from(std::string("mytable")));
}
