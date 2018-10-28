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

#include <sqlpp17_test/float_test.h>

#include <sqlpp17/postgresql/connection.h>

namespace
{
  using test::tabFloat;

  auto print_debug(std::string_view message)
  {
    std::cout << "Debug: " << message << std::endl;
  }

#warning: should generalize tests for accepted/rejected float/double values
  template <typename Db>
  auto testSpecialValues(Db& db) -> void
  {
    std::cout << "Testing special values for insertion...";
    auto preparedInsert =
        db.prepare(insert_into(tabFloat).set(tabFloat.valueFloat = ::sqlpp::parameter<float>(tabFloat.valueFloat),
                                             tabFloat.valueDouble = ::sqlpp::parameter<double>(tabFloat.valueDouble),
                                             tabFloat.valueInt = ::sqlpp::parameter<std::int32_t>(tabFloat.valueInt)));
    preparedInsert.parameters.valueFloat = std::nanf("");
    preparedInsert.parameters.valueDouble = std::nan("");
    execute(preparedInsert);
  }
}

namespace postgresql = sqlpp::postgresql;
int main()
{
  auto config = postgresql::connection_config_t{};
#warning: Need to streamline the db name and the error messages
  config.dbname = "sqlpp17_test";
  config.debug = print_debug;
  try
  {
    auto db = postgresql::connection_t<::sqlpp::debug::none>{config};
  }
  catch (const sqlpp::exception& e)
  {
    std::cerr << "For testing, you'll need to create a database sqlpp_test for user root (no password)" << std::endl;
    std::cerr << e.what() << std::endl;
    return 1;
  }

  try
  {
    auto db = postgresql::connection_t<::sqlpp::debug::none>{config};
    db(drop_table(tabFloat));
    db(create_table(tabFloat));

    std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
    sqlpp::test::testDirectExecution(db);
    sqlpp::test::testPreparedExecution(db);
    testSpecialValues(db);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

