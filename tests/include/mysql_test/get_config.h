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

#include <sqlpp17/mysql/connection.h>

namespace sqlpp::mysql::test
{
  auto print_debug(std::string_view message)
  {
    std::cout << "Debug: " << message << std::endl;
  }

  auto get_config() -> ::sqlpp::mysql::connection_config_t
  {
    auto config = ::sqlpp::mysql::connection_config_t{};
    config.user = "root";
#warning : This needs to be configurable
    config.password = "";
    config.database = "core_test";
    config.debug = print_debug;
    try
    {
      auto db = ::sqlpp::mysql::connection_t<::sqlpp::debug::none>{config};
    }
    catch (const sqlpp::exception& e)
    {
      std::cerr << "For testing, you'll need to create a database " << config.database << " for user root (no password)"
                << std::endl;
      throw;
    }
    return config;
  }
}  // namespace sqlpp::mysql::test
