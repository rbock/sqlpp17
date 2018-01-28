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

#include <string>

#include <sqlpp17/clause/select.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/parameter.h>

#include <sqlpp17_test/mock_db.h>
#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabEmpty.h>
#include <sqlpp17_test/tables/TabPerson.h>

#include "assert_bad_expression.h"

using ::sqlpp::test::assert_bad_expression;
using ::sqlpp::test::assert_good_expression;

// Turning off static_assert for statements
namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_statement_all_required_tables_are_provided, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_execute_without_parameters, T...> = true;
}  // namespace sqlpp

namespace
{
  SQLPP_CREATE_NAME_TAG(foo);
}  // namespace

int main()
{
  auto db = ::sqlpp::test::mock_db{};

  auto pi = test::tabPerson.as(foo);
  auto qu = test::tabDepartment.as(foo);

  // bad: using two table aliases with the same name
  assert_bad_expression(sqlpp::assert_statement_all_required_tables_are_provided{},
                        db(select(all_of(pi)).from(qu).unconditionally()));

  // bad: using a parameter in a directly executed statement
  assert_bad_expression(::sqlpp::assert_execute_without_parameters{}, db(select(::sqlpp::parameter<int>(foo).as(foo))));
}
