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

#include <sqlpp17/postgresql/connection.h>
#include <sqlpp17/postgresql_test/get_config.h>

#include <sqlpp17_test/tables/TabDepartment.h>

namespace postgresql = sqlpp::postgresql;
int main()
{
  try
  {
    const auto config = postgresql::test::get_config();
    auto db = postgresql::connection_t<::sqlpp::debug::allowed>{config};
    db(drop_table(test::tabDepartment));
    db(create_table(test::tabDepartment));

    auto id = db(insert_into(test::tabDepartment).default_values());
    id = db(insert_into(test::tabDepartment).set(test::tabDepartment.name = "hansi"));

    auto prepared_select = db.prepare(
        sqlpp::select(test::tabDepartment.id, test::tabDepartment.name).from(test::tabDepartment).unconditionally());
    for (const auto& row : execute(prepared_select))
    {
      std::cout << row.id << ", " << row.name.value_or("NULL") << std::endl;
    }
    for (const auto& row : execute(prepared_select))
    {
      std::cout << row.id << ", " << row.name.value_or("NULL") << std::endl;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

