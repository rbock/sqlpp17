#pragma once

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
#include <sqlpp17/clause/select.h>
#include <sqlpp17/clause/truncate.h>
#include <sqlpp17/parameter.h>

#include <core_test/compare.h>
#include <core_test/tables/TabFloat.h>

namespace sqlpp::test
{
  struct Row
  {
    float valueFloat;
    double valueDouble;
    std::int32_t valueInt;
  };

  const auto inputRows = std::vector<Row>{{12345678901234567890., 12345678901234567890., 1234567890},
                                          {-12345678901234567890., -12345678901234567890., -1234567890},
                                          {0., 0., 0},
                                          {1.2345678901234567890, 1.2345678901234567890, 0},
                                          {-1.2345678901234567890, -1.2345678901234567890, 0},
                                          {DBL_MIN / 2.0, DBL_MIN, 0}};

  template <typename Db>
  auto testDirectExecution(Db& db) -> void
  {
    std::cout << "Comparing inserted and retrieved values in direct execution...";

    using ::test::tabFloat;

    db(truncate(tabFloat));
    for (const auto& input : inputRows)
    {
      [[maybe_unused]] auto id =
          db(insert_into(tabFloat).set(tabFloat.valueFloat = input.valueFloat, tabFloat.valueDouble = input.valueDouble,
                                       tabFloat.valueInt = input.valueInt));
    }

    auto index = std::size_t{};
    for (const auto& row : db(select(all_of(tabFloat)).from(tabFloat).unconditionally().order_by(tabFloat.id.asc())))
    {
      // std::cout << row.id << " " << row.valueFloat << ", " << row.valueDouble << ", " << row.valueInt << std::endl;
      ::sqlpp::test::compare(index, inputRows[index].valueFloat, row.valueFloat);
      ::sqlpp::test::compare(index, inputRows[index].valueDouble, row.valueDouble);
      ::sqlpp::test::compare(index, inputRows[index].valueInt, row.valueInt);
      ++index;
    }
    std::cout << " OK" << std::endl;
  }

  template <typename Db>
  auto testPreparedExecution(Db& db) -> void
  {
    std::cout << "Comparing inserted and retrieved values in prepared statements with parameters for insert...";

    using ::test::tabFloat;

    db(truncate(tabFloat));
    auto preparedInsert =
        db.prepare(insert_into(tabFloat).set(tabFloat.valueFloat = ::sqlpp::parameter<float>(tabFloat.valueFloat),
                                             tabFloat.valueDouble = ::sqlpp::parameter<double>(tabFloat.valueDouble),
                                             tabFloat.valueInt = ::sqlpp::parameter<std::int32_t>(tabFloat.valueInt)));
    for (const auto& input : inputRows)
    {
      preparedInsert.parameters.valueFloat = input.valueFloat;
      preparedInsert.parameters.valueDouble = input.valueDouble;
      preparedInsert.parameters.valueInt = input.valueInt;
      execute(preparedInsert);
    }
    auto index = std::size_t{};
    for (const auto& row : db(select(all_of(tabFloat)).from(tabFloat).unconditionally().order_by(tabFloat.id.asc())))
    {
      // std::cout << row.id << " " << row.valueFloat << ", " << row.valueDouble << ", " << row.valueInt << std::endl;
      ::sqlpp::test::compare(index, inputRows[index].valueFloat, row.valueFloat);
      ::sqlpp::test::compare(index, inputRows[index].valueDouble, row.valueDouble);
      ::sqlpp::test::compare(index, inputRows[index].valueInt, row.valueInt);
      ++index;
    }
    std::cout << " OK" << std::endl;
  }

}

