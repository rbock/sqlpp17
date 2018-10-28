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

#include <sqlpp17/clause/select.h>
#include <sqlpp17/clause/with.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/result_cast.h>
#include <sqlpp17/value.h>

#include <sqlpp17/sqlite3/connection.h>
#include <sqlpp17/sqlite3_test/get_config.h>

namespace
{
  namespace alias
  {
    SQLPP_CREATE_NAME_TAG(cnt);
    SQLPP_CREATE_NAME_TAG(x);
  }  // namespace alias
}  // namespace

int main()
{
  try
  {
    const auto config = ::sqlpp::sqlite3::test::get_config();
    auto db = ::sqlpp::sqlite3::connection_t<::sqlpp::debug::allowed>{config};

    /* Example from sqlite https://sqlite.org/lang_with.html

      ```
      WITH RECURSIVE
      cnt(x) AS (VALUES(1) UNION ALL SELECT x+1 FROM cnt WHERE x<1000000)
      SELECT x FROM cnt;
      ```
    */
    const auto cnt = [&]() {
      auto cnt = cte(alias::cnt).as(select(as(::sqlpp::value(int32_t(1)), alias::x)));
      return cnt.union_all(select(as(::sqlpp::result_cast<int32_t>(cnt.x + 1), alias::x)).from(cnt).where(cnt.x < 10));
    }();
    for (const auto& row : db(with_recursive(cnt) << select(cnt.x).from(cnt).unconditionally()))
    {
      std::cout << row.x << '\n';
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

