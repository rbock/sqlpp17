/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp17/core/case.h>
#include <sqlpp17/core/operator.h>

#include <core_test/tables/TabDepartment.h>
#include <core_test/tables/TabEmpty.h>
#include <core_test/tables/TabPerson.h>

#include <core_test/mock_db.h>

#include "assert_equality.h"

using ::sqlpp::test::assert_equality;
using ::sqlpp::test::mock_context_t;

int main()
{
  using test::tabPerson;
  constexpr auto c = case_when(tabPerson.id % 3 == 2, then(tabPerson.id > 7))
                         .when(tabPerson.id % 3 == 1, then(tabPerson.id > 9))
                         .else_(tabPerson.id > 17);
  try
  {
    assert_equality(" CASE "
                    "WHEN ((tab_person.id % 3) = 2) THEN (tab_person.id > 7) "
                    "WHEN ((tab_person.id % 3) = 1) THEN (tab_person.id > 9) "
                    "ELSE (tab_person.id > 17)",
                    to_sql_string_c(mock_context_t{}, c));
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << '\n';
    return 1;
  }
}
