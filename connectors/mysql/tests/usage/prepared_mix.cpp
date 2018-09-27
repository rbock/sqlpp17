/*
Copyright (c) 2018 - 2018, Roland Bock
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
#include <sqlpp17/clause/update.h>
#include <sqlpp17/clause/select.h>

#include <sqlpp17/mysql/connection.h>

#include <sqlpp17_test/tables/TabDepartment.h>

auto print_debug(std::string_view message)
{
  std::cout << "Debug: " << message << std::endl;
}

namespace mysql = sqlpp::mysql;
int main()
{
  mysql::global_library_init();

  auto config = mysql::connection_config_t{};
  config.user = "root";
#warning : This needs to be configurable
  config.password = "";
  config.database = "sqlpp_mysql";
  config.debug = print_debug;
  try
  {
    auto db = mysql::connection_t<sqlpp::debug::none>{config};
  }
  catch (const sqlpp::exception& e)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_mysql for user root (no password)" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  try
  {
    auto db = mysql::connection_t<sqlpp::debug::allowed>{config};
    db(drop_table(test::tabDepartment));
    db(create_table(test::tabDepartment));

    auto id = db(insert_into(test::tabDepartment).default_values());
    id = db(insert_into(test::tabDepartment).set(test::tabDepartment.name = "hansi"));

    auto prepared_select = db.prepare(
        sqlpp::select(test::tabDepartment.id).from(test::tabDepartment).unconditionally());

    std::ignore = execute(prepared_select).front().id; // This does not read to the end
    prepared_select.clear_result();

    // This fails if the result does not call mysql_stmt_free_result in the destructor
    auto prepared_update = db.prepare(update(test::tabDepartment).set(test::tabDepartment.name = "hansi").unconditionally());
    execute(prepared_update);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

