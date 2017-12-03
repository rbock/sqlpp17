/*
Copyright (c) 2017, Roland Bock
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

#include <sqlpp17/clauses/insert.h>

#include <sqlpp17/mysql/connection.h>

#include <tables/TabDepartment.h>

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
  config.password = "test";
  config.database = "sqlpp_mysql";
  config.debug = print_debug;
  try
  {
    auto db = mysql::connection_t{config};
  }
  catch (const sqlpp::exception& e)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_mysql for user root (no password)" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }
  try
  {
    auto db = mysql::connection_t{config};
    db.execute(R"(DROP TABLE IF EXISTS tab_department)");
    db.execute(R"(CREATE TABLE tab_department (
			id bigint(20) AUTO_INCREMENT,
			name varchar(255) NOT NULL,
			PRIMARY KEY (id)
			))");

    auto id = db(sqlpp::insert().into(test::tabDepartment).default_values());
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

