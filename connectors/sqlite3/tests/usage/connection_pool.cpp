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
#include <mutex>
#include <random>
#include <set>
#include <thread>

#include <sqlpp17/clause/create_table.h>
#include <sqlpp17/clause/drop_table.h>
#include <sqlpp17/clause/insert_into.h>

#include <sqlpp17/sqlite3/connection_pool.h>
#include <sqlpp17/sqlite3_test/get_config.h>

#include <sqlpp17_test/tables/TabDepartment.h>

namespace
{
  auto pool = ::sqlpp::sqlite3::connection_pool_t<::sqlpp::debug::allowed>{5, ::sqlpp::sqlite3::test::get_config()};
}  // namespace

[[nodiscard]] auto test_basic_functionality()
{
  try
  {
    auto db = pool.get();
    db(drop_table(test::tabDepartment));
    db(create_table(test::tabDepartment));

    [[maybe_unused]] auto id = db(insert_into(test::tabDepartment).default_values());
  }
  catch (const std::exception& e)
  {
    std::cerr << std::string(__func__) + "Exception: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

[[nodiscard]] auto test_single_connection()
{
  ::sqlite3* handle = nullptr;
  {
    auto db = pool.get();
    handle = db.get();
  }

  for (auto i = 0; i < 100; ++i)
  {
    auto db = pool.get();
    if (handle != db.get())
    {
      std::cerr << std::string(__func__) + ": Pool acquired more than one connection " << handle << "!=" << db.get()
                << "\n";
      return 1;
    }
  }
  return 0;
}

[[nodiscard]] auto test_multiple_connections()
{
  auto connections = std::vector<std::decay_t<decltype(pool.get())>>{};
  auto pointers = std::set<void*>{};
  try
  {
    for (auto i = 0; i < 100; ++i)
    {
      connections.push_back(pool.get());
      if (pointers.count(connections.back().get()))
      {
        std::cerr << __func__ << ": Pool yielded connection twice (without getting it back in between): "
                  << connections.back().get() << "\n";
      }
      pointers.insert(connections.back().get());
      [[maybe_unused]] auto id = connections.back()(insert_into(test::tabDepartment).default_values());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << std::string(__func__) + ": Caught excetion: " << e.what() << "\n";
    return 1;
  }
  return 0;
}

namespace
{
  std::random_device r;
  std::default_random_engine random_engine(r());
  std::uniform_int_distribution<int> uniform_dist(1, 20);
}  // namespace

[[nodiscard]] auto test_multithreaded()
{
  if (not sqlite3_threadsafe())
  {
    std::clog << "sqlite3 not compiled with thread safety.\n";
    std::clog << "Not running multi-threaded tests.\n";
    return 0;
  }
  std::clog << "Run a random number [1,20] of threads\n";
  std::clog << "Each with a random number [1,20] of {pool.get() & insert}\n";

  auto threads = std::vector<std::thread>{};
  const auto thread_count = uniform_dist(random_engine);
  auto statement_mutex = std::mutex{};

  for (auto i = 0; i < thread_count; ++i)
  {
    threads.push_back(std::thread([func = __func__, call_count = uniform_dist(random_engine), &statement_mutex]() {
      try
      {
        for (auto k = 0; k < call_count; ++k)
        {
          auto connection = pool.get();
          const auto lock = std::scoped_lock(statement_mutex);
          [[maybe_unused]] auto id = connection(insert_into(test::tabDepartment).default_values());
        }
      }
      catch (const std::exception& e)
      {
        std::cerr << std::string(func) + ": In-thread exception: " + e.what() + "\n";
        std::abort();
      }
    }));
  }
  for (auto&& t : threads)
  {
    t.join();
  }
  return 0;
}

int main()
{
  return test_basic_functionality() or test_single_connection() or test_multiple_connections() or test_multithreaded();
}

