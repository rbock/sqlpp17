/*
Copyright (c) 2016 - 2019, Roland Bock
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

#include <sqlpp17/core/clause/insert_into.h>
#include <sqlpp17/core/operator.h>

#include <core_test/mock_db.h>
#include <core_test/tables/TabDepartment.h>
#include <core_test/tables/TabEmpty.h>
#include <core_test/tables/TabPerson.h>

#include "assert_equality.h"

using ::sqlpp::test::assert_equality;
using ::sqlpp::test::mock_context_t;
using test::tabDepartment;
using test::tabPerson;

int main()
{
  // standard way of constructing an insert statement
  assert_equality("INSERT INTO tab_department DEFAULT VALUES",
                  to_sql_string_c(mock_context_t{}, insert_into(tabDepartment).default_values()));
  assert_equality(
      "INSERT INTO tab_department (name) VALUES ('Engineering')",
      to_sql_string_c(mock_context_t{}, insert_into(tabDepartment).set(tabDepartment.name = "Engineering")));
  assert_equality("INSERT INTO tab_department (name) "
                  "VALUES ('Engineering'), ('Marketing'), ('Sales')",
                  to_sql_string_c(mock_context_t{}, insert_into(tabDepartment)
                                                        .multiset(std::vector{
                                                            std::tuple{tabDepartment.name = "Engineering"},
                                                            std::tuple{tabDepartment.name = "Marketing"},
                                                            std::tuple{tabDepartment.name = "Sales"},
                                                        })));

  assert_equality("INSERT INTO tab_person (is_manager, name) "
                  "VALUES (1, 'Sample Name')",
                  to_sql_string_c(mock_context_t{}, insert_into(tabPerson).set(tabPerson.isManager = true,
                                                                               tabPerson.name = "Sample Name")));
  assert_equality("INSERT INTO tab_person (is_manager, name, address) "
                  "VALUES (1, 'Sample Name', 'Sample Address')",
                  to_sql_string_c(mock_context_t{},
                                  insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                                             tabPerson.address = "Sample Address")));
  assert_equality("INSERT INTO tab_person (is_manager, name, address) "
                  "VALUES (1, 'Sample Name', 'Sample Address')",
                  to_sql_string_c(mock_context_t{},
                                  insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                                             tabPerson.address = "Sample Address")));
  assert_equality(
      "INSERT INTO tab_person (is_manager, name) "
      "VALUES (0, 'Mr. C++'), (1, 'Mr. CEO')",
      to_sql_string_c(mock_context_t{}, insert_into(tabPerson).multiset(std::vector{
                                            std::tuple{tabPerson.isManager = false, tabPerson.name = "Mr. C++"},
                                            std::tuple{tabPerson.isManager = true, tabPerson.name = "Mr. CEO"}})));

  // For columns with a default value, you can use std::optional to either pass a specific value or the default
  assert_equality("INSERT INTO tab_person (is_manager, name, address) "
                  "VALUES (1, 'Sample Name', 'Sample Address')",
                  to_sql_string_c(mock_context_t{},
                                  insert_into(tabPerson).set(
                                      tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                      true ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt)));
  assert_equality("INSERT INTO tab_person (is_manager, name, address, language) "
                  "VALUES (1, 'Sample Name', DEFAULT, DEFAULT)",
                  to_sql_string_c(mock_context_t{},
                                  insert_into(tabPerson).set(
                                      tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                      false ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt,
                                      false ? std::make_optional(tabPerson.language = "Java") : std::nullopt)));

  assert_equality(
      "INSERT INTO tab_person (is_manager, name, address, language) "
      "VALUES (0, 'Mr. C++', DEFAULT, DEFAULT), (1, 'Mr. CEO', 'Sample Address', DEFAULT), "
      "(0, 'Mr. C++', DEFAULT, 'Python'), (1, 'Mr. CEO', 'Sample Address', 'Python')",
      to_sql_string_c(mock_context_t{},
                      insert_into(tabPerson).multiset(std::vector{
                          std::tuple{tabPerson.isManager = false, tabPerson.name = "Mr. C++",
                                     false ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt,
                                     false ? std::make_optional(tabPerson.language = "Python") : std::nullopt},
                          std::tuple{tabPerson.isManager = true, tabPerson.name = "Mr. CEO",
                                     true ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt,
                                     false ? std::make_optional(tabPerson.language = "Python") : std::nullopt},
                          std::tuple{tabPerson.isManager = false, tabPerson.name = "Mr. C++",
                                     false ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt,
                                     true ? std::make_optional(tabPerson.language = "Python") : std::nullopt},
                          std::tuple{tabPerson.isManager = true, tabPerson.name = "Mr. CEO",
                                     true ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt,
                                     true ? std::make_optional(tabPerson.language = "Python") : std::nullopt}})));
}
