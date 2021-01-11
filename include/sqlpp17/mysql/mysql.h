#pragma once

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

#include <mysql.h>

#include <sqlpp17/core/exception.h>

namespace sqlpp::mysql::detail
{
#warning : This should go into a separate file
#if LIBMYSQL_VERSION_ID >= 80000
  using my_bool = bool;
#endif

  class scoped_library_initializer_t
  {
  public:
    scoped_library_initializer_t(int argc, char** argv, char** groups)
    {
      mysql_library_init(argc, argv, groups);
    }

    ~scoped_library_initializer_t()
    {
      mysql_library_end();
    }
  };

  struct MySqlThreadInitializer
  {
    MySqlThreadInitializer()
    {
      if (!mysql_thread_safe())
      {
        throw sqlpp::exception("MySQL: Operating on a non-threadsafe client");
      }
      mysql_thread_init();
    }

    ~MySqlThreadInitializer()
    {
      mysql_thread_end();
    }
  };

#ifdef __APPLE__
  boost::thread_specific_ptr<MySqlThreadInitializer> mysqlThreadInit;
  inline auto thread_init() -> void
  {
    if (!mysqlThreadInit.get())
    {
      mysqlThreadInit.reset(new MySqlThreadInitializer);
    }
  }
#else
  inline auto thread_init() -> void
  {
    thread_local MySqlThreadInitializer threadInitializer;
  }
#endif

}  // namespace sqlpp::mysql::detail
