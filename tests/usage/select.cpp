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

#include <sqlpp17_test/mock_db.h>
#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabEmpty.h>
#include <sqlpp17_test/tables/TabPerson.h>

#include <sqlpp17/clause/select.h>
#include <sqlpp17/function.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/sql_cast.h>

using test::tabPerson;

template <typename Row>
auto display_full_row(const Row& row)
{
  std::cout << row.id << "\n";
  std::cout << row.isManager << "\n";
  std::cout << row.name << "\n";
  std::cout << row.address.value_or("") << "\n";
}

SQLPP_CREATE_NAME_TAG(rowCount);
SQLPP_CREATE_NAME_TAG(maxName);
SQLPP_CREATE_NAME_TAG(avgId);

int main()
{
  auto db = ::sqlpp::test::mock_db{};

  // A simple select from without condition
  for (const auto& row : db(select(all_of(tabPerson)).from(tabPerson)))
  {
    display_full_row(row);
  }

  // A simple select from explicitly without condition
  for (const auto& row : db(select(all_of(tabPerson)).from(tabPerson).unconditionally()))
  {
    display_full_row(row);
  }

  // A simple select from where
  for (const auto& row : db(select(all_of(tabPerson)).from(tabPerson).where(tabPerson.isManager)))
  {
    display_full_row(row);
  }

  for (const auto& row : db(select(::sqlpp::count(1).as(rowCount), max(tabPerson.name).as(maxName),
                                   avg(tabPerson.id).as(avgId), tabPerson.isManager)
                                .from(tabPerson)
                                .where(tabPerson.isManager and tabPerson.name != "")
                                .group_by(tabPerson.isManager)
                                .having(::sqlpp::count(1) > 7)
                                .order_by(asc(max(tabPerson.id)))
                                .limit(1)
                                .offset(1)))
  {
    std::cout << row.rowCount << std::endl;
    std::cout << row.maxName << std::endl;
    std::cout << row.avgId << std::endl;
  }

#warning : Need more examples

  // using << concatenation
  for (const auto& row :
       db(::sqlpp::select() << select_columns(::sqlpp::count(1).as(rowCount), max(tabPerson.name).as(maxName),
                                              avg(tabPerson.id).as(avgId), tabPerson.isManager)
                            << from(tabPerson) << where(tabPerson.isManager and tabPerson.name != "")
                            << group_by(tabPerson.isManager) << having(::sqlpp::count(1) > 7)
                            << order_by(asc(max(tabPerson.id)))))
  {
    std::cout << row.rowCount << std::endl;
    std::cout << row.maxName << std::endl;
    std::cout << row.avgId << std::endl;
  }
}
