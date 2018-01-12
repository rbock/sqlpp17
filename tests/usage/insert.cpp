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

#include <sqlpp17/clause/insert_into.h>
#include <sqlpp17/operator.h>

#include <connections/mock_db.h>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

using test::tabDepartment;
using test::tabPerson;

static_assert(::sqlpp::required_insert_columns_of_f(tabDepartment) == ::sqlpp::type_set());
static_assert(::sqlpp::required_insert_columns_of_f(tabPerson) ==
              ::sqlpp::type_set(tabPerson.isManager, tabPerson.name));

int main()
{
  auto db = ::sqlpp::test::mock_db{};
  auto id = std::size_t{};
  auto hasAddress = false;

  // standard way of constructing an insert statement
  id = db(insert_into(tabDepartment).default_values());
  id = db(insert_into(tabDepartment).set(tabDepartment.name = "Engineering"));
  id = db(insert_into(tabDepartment)
              .multiset(std::vector{
                  std::tuple{tabDepartment.name = "Engineering"},
                  std::tuple{tabDepartment.name = "Marketing"},
                  std::tuple{tabDepartment.name = "Sales"},
              }));

  id = db(insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name"));
  id = db(insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                     tabPerson.address = "Sample Address"));
  id = db(insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                     tabPerson.address = "Sample Address"));
  id = db(insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                     tabPerson.address = ::std::nullopt, tabPerson.language = "C++"));
  id = db(insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                     tabPerson.address = ::std::optional<std::string_view>{},
                                     tabPerson.language = "C++"));
  id = db(
      insert_into(tabPerson).multiset(std::vector{std::tuple{tabPerson.isManager = false, tabPerson.name = "Mr. C++"},
                                                  std::tuple{tabPerson.isManager = true, tabPerson.name = "Mr. CEO"}}));

  // For columns with a default value, you can use std::optional to either pass a specific value or the default
  id = db(
      insert_into(tabPerson).set(tabPerson.isManager = true, tabPerson.name = "Sample Name",
                                 hasAddress ? std::make_optional(tabPerson.address = "Sample Address") : std::nullopt));
}
