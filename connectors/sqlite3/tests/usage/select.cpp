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

#include <sqlpp17/clause/create_table.h>
#include <sqlpp17/clause/drop_table.h>
#include <sqlpp17/clause/insert_into.h>
#include <sqlpp17/clause/select.h>
#include <sqlpp17/function.h>

#include <sqlpp17/sqlite3/connection.h>

#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabPerson.h>

using test::tabDepartment;
using test::tabPerson;

auto print_debug(std::string_view message)
{
  std::cout << "Debug: " << message << std::endl;
}

SQLPP_CREATE_NAME_TAG(rowCount);
SQLPP_CREATE_NAME_TAG(maxName);
SQLPP_CREATE_NAME_TAG(avgId);

int main()
{
  auto config = ::sqlpp::sqlite3::connection_config_t{};
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = print_debug;

  try
  {
    auto db = ::sqlpp::sqlite3::connection_t{config};
    db(drop_table(tabDepartment));
    db(drop_table(tabPerson));
    db(create_table(tabDepartment));
    db(create_table(tabPerson));

    auto id = db(insert_into(tabDepartment).default_values());
    id = db(insert_into(tabDepartment).set(tabDepartment.name = "hansi"));

    for (const auto& row :
         db(sqlpp::select(tabDepartment.id, tabDepartment.name).from(tabDepartment).unconditionally()))
    {
      std::cout << row.id << ", " << row.name.value_or("NULL") << std::endl;
    }

    for (const auto& row : db(select(::sqlpp::count(1).as(rowCount), max(tabPerson.name).as(maxName),
                                     avg(tabPerson.id).as<float>(avgId), tabPerson.isManager)
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
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

