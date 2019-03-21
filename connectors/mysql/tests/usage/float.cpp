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

#include <sqlpp17/mysql/connection.h>
#include <sqlpp17/mysql_test/get_config.h>

namespace
{
  using ::test::tabFloat;

  template <typename Db>
  auto testInvalidValues(Db& db) -> void
  {
    std::cout << "Testing invalid values for insertion...";
    auto preparedInsert =
        db.prepare(insert_into(tabFloat).set(tabFloat.valueFloat = ::sqlpp::parameter<float>(tabFloat.valueFloat),
                                             tabFloat.valueDouble = ::sqlpp::parameter<double>(tabFloat.valueDouble),
                                             tabFloat.valueInt = ::sqlpp::parameter<std::int32_t>(tabFloat.valueInt)));
    try
    {
      preparedInsert.parameters.valueFloat = std::nanf("");
      preparedInsert.parameters.valueDouble = std::nan("");
      execute(preparedInsert);
      throw std::domain_error("Unexpected acceptance of std::nan and std:nanf");
    }
    catch (const ::sqlpp::exception& e)
    {
      // expected exception
    }
    std::cout << " OK (still invalid)" << std::endl;
  }
}  // namespace

namespace mysql = sqlpp::mysql;
int main()
{
  try
  {
    mysql::global_library_init();

    const auto config = mysql::test::get_config();
    auto db = mysql::connection_t<sqlpp::debug::none>{config};
    db(drop_table(tabFloat));
    db(create_table(tabFloat));

    std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);

    sqlpp::test::testDirectExecution(db);
    sqlpp::test::testPreparedExecution(db);
#warning: NAN seems to be a moving target. Inserting failed half a year ago, but after a system update, it succeeds.
    //testInvalidValues(db);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

