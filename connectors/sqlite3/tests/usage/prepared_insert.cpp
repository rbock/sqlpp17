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

#include <sqlpp17/sqlite3/connection.h>

#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabPerson.h>

using test::tabDepartment;
using test::tabPerson;

SQLPP_CREATE_NAME_TAG(pName);
SQLPP_CREATE_NAME_TAG(pIsManager);
SQLPP_CREATE_NAME_TAG(pAddress);

auto print_debug(std::string_view message)
{
  std::cout << "Debug: " << message << std::endl;
}

int main()
{
  auto config = ::sqlpp::sqlite3::connection_config_t{};
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  config.debug = print_debug;

  try
  {
    auto db = ::sqlpp::sqlite3::connection_t<::sqlpp::debug::allowed>{config};
    db(drop_table(tabDepartment));
    db(drop_table(tabPerson));
    db(create_table(tabDepartment));
    db(create_table(tabPerson));

    {
      auto prepared_insert = db.prepare(insert_into(tabDepartment).default_values());
      [[maybe_unused]] const auto id = execute(prepared_insert);
    }

    {
      auto s = db.prepare(insert_into(tabPerson).set(tabPerson.isManager = true,
                                                     tabPerson.name = ::sqlpp::parameter<std::string>(pName)));
      s.parameters.pName = "Herb";
      [[maybe_unused]] const auto id = execute(s);
    }

    {
      auto s = db.prepare(insert_into(tabPerson).set(
          tabPerson.isManager = ::sqlpp::parameter<bool>(pIsManager),
          tabPerson.name = ::sqlpp::parameter<std::string>(pName),
          tabPerson.address = ::sqlpp::parameter<::std::optional<std::string>>(pAddress), tabPerson.language = "C++"));
      s.parameters.pIsManager = true;
      s.parameters.pName = "Herb";
      s.parameters.pAddress = "Somewhere";
      s.parameters.pAddress = std::nullopt;
      s.parameters.pAddress.reset();

      [[maybe_unused]] const auto id = execute(s);
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

