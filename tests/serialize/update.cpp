/*
Copyright (c) 2016 - 2018, Roland Bock
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

#include <sqlpp17/clause/update.h>
#include <sqlpp17/operator.h>

#include <connections/mock_db.h>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

#include <serialize/assert_equality.h>

using ::sqlpp::test::assert_equality;
using ::sqlpp::test::mock_context_t;
using ::test::tabPerson;

int main()
{
  // normal updates
  assert_equality(
      "UPDATE tab_person SET is_manager = 1",
      to_sql_string_c(mock_context_t{}, update(tabPerson).set(tabPerson.isManager = true).unconditionally()));
  assert_equality(
      "UPDATE tab_person SET is_manager = 1 WHERE tab_person.is_manager = 0",
      to_sql_string_c(mock_context_t{},
                      update(tabPerson).set(tabPerson.isManager = true).where(tabPerson.isManager == false)));

  // update with optional assignment
  assert_equality(
      "UPDATE tab_person SET is_manager = 1, name = 'New Name'",
      to_sql_string_c(mock_context_t{}, update(tabPerson)
                                            .set(tabPerson.isManager = true,
                                                 true ? std::make_optional(tabPerson.name = "New Name") : std::nullopt)
                                            .unconditionally()));
  assert_equality(
      "UPDATE tab_person SET is_manager = 1, name = name",
      to_sql_string_c(mock_context_t{}, update(tabPerson)
                                            .set(tabPerson.isManager = true,
                                                 false ? std::make_optional(tabPerson.name = "New Name") : std::nullopt)
                                            .unconditionally()));

  // query concatenation
  assert_equality("UPDATE tab_person SET is_manager = 1",
                  to_sql_string_c(mock_context_t{}, update(tabPerson) << update_set(tabPerson.isManager = true)
                                                                      << sqlpp::unconditionally()));
  assert_equality("UPDATE tab_person SET is_manager = 1 "
                  "WHERE tab_person.is_manager = 0",
                  to_sql_string_c(mock_context_t{}, update(tabPerson) << update_set(tabPerson.isManager = true)
                                                                      << where(tabPerson.isManager == false)));
}
