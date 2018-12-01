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

#include <sqlpp17/sqlite3/connection_pool.h>
#include <sqlpp17/sqlite3_test/get_config.h>

#include <sqlpp17_test/connection_pool_tests.h>

namespace
{
  auto post_connect(::sqlite3* db) -> void
  {
    const auto rc = sqlite3_exec(db, "pragma busy_timeout=30000", nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK)
    {
      throw std::runtime_error("Could not set busy timeout: " + std::string(sqlite3_errmsg(db)));
    }
  };
}
int main()
{
  try
  {
    auto config = ::sqlpp::sqlite3::test::get_config();
    config.post_connect = post_connect;
    auto pool = ::sqlpp::sqlite3::connection_pool_t<::sqlpp::debug::allowed>{5, config};

    ::sqlpp::test::test_basic_functionality(pool);
    ::sqlpp::test::test_single_connection(pool);
    ::sqlpp::test::test_multiple_connections(pool);

    if (sqlite3_threadsafe())
    {
      ::sqlpp::test::test_multithreaded(pool);
    }
    else
    {
      std::clog << "sqlite3 not compiled with thread safety.\n";
      std::clog << "Not running multi-threaded tests.\n";
    }

  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << std::endl;
    return 1;
  }
}

