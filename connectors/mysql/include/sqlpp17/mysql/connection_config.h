#pragma once

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

#include <optional>

#include <mysql/mysql.h>

namespace sqlpp::mysql
{
  struct ssl_config_t
  {
    std::string key;
    std::string cert;
    std::string ca;
    std::string caPath;
    std::string cipher;
  };

  struct connection_config_t
  {
    std::function<void(MYSQL*)> pre_connect;
    std::function<void(MYSQL*)> post_connect;
    std::string host;
    std::string user;
    std::string password;
    int port = 0;
    std::string unix_socket;
    unsigned long client_flag = 0;
    std::optional<ssl_config_t> ssl;

    connection_config_t() = default;
    connection_config_t(const connection_config_t&) = default;
    connection_config_t(connection_config_t&& rhs) = default;
    connection_config_t& operator=(const connection_config_t&) = default;
    connection_config_t& operator=(connection_config_t&&) = default;
    ~connection_config_t() = default;
  };

}  // namespace sqlpp::mysql
