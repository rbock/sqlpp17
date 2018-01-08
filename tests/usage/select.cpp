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

#include <iostream>

#include <connections/mock_db.h>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

#include <sqlpp17/clause/select.h>
#include <sqlpp17/operator.h>

using test::tabPerson;

template <typename Row>
auto display_full_row(const Row& row)
{
  std::cout << row.id << "\n";
  std::cout << row.isManager << "\n";
  std::cout << row.name << "\n";
  std::cout << row.address.value_or("") << "\n";
}

int main()
{
  auto db = ::sqlpp::test::mock_db{};

  for (const auto& row : db(sqlpp::select(all_of(tabPerson)).from(tabPerson).where(tabPerson.isManager)))
  {
    display_full_row(row);
  }

#warning : Need more examples

  // using << concatenation
  for (const auto& row :
       db(sqlpp::select() << sqlpp::select_columns(test::tabPerson.id, test::tabPerson.isManager,
                                                   test::tabPerson.address, test::tabPerson.name)
                          << sqlpp::from(test::tabPerson)
                          << sqlpp::where(test::tabPerson.isManager and test::tabPerson.name == "")
                          << sqlpp::having(test::tabPerson.id == test::tabPerson.id or test::tabPerson.id == 1)))
  {
    std::cout << row.id << std::endl;
    std::cout << row.isManager << std::endl;
    std::cout << row.name << std::endl;
    std::cout << row.address.value_or("") << std::endl;
  }
}
