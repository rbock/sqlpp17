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

#include <libpq-fe.h>

namespace sqlpp::postgresql
{
  struct connection_config_t
  {
    std::function<void(PGconn*)> pre_connect;
    std::function<void(PGconn*)> post_connect;

    // see https://www.postgresql.org/docs/10/static/libpq-connect.html
    std::optional<std::string> host;
    std::optional<std::string> hostaddr;
    std::optional<std::string> port;
    std::optional<std::string> dbname;
    std::optional<std::string> user;
    std::optional<std::string> password;
    std::optional<std::string> passfile;
    std::optional<int> connect_timeout;
    std::optional<std::string> client_encoding = "utf8";
    std::optional<std::string> options;
    std::optional<std::string> application_name;
    std::optional<std::string> fallback_application_name;
    std::optional<bool> keepalives = true;
    std::optional<int> keepalives_idle;
    std::optional<int> keepalives_interval;
    std::optional<int> keepalives_count;
    std::optional<std::string> sslmode;
    std::optional<bool> sslcompression;
    std::optional<std::string> sslcert;
    std::optional<std::string> sslkey;
    std::optional<std::string> sslrootcert;
    std::optional<std::string> sslcrl;
    std::optional<std::string> requirepeer;
    std::optional<std::string> krbsrvname;
    std::optional<std::string> gsslib;
    std::optional<std::string> service;
    std::optional<std::string> target_session_attrs;

    std::function<void(std::string_view)> debug;

    connection_config_t() = default;
    connection_config_t(const connection_config_t&) = default;
    connection_config_t(connection_config_t&& rhs) = default;
    connection_config_t& operator=(const connection_config_t&) = default;
    connection_config_t& operator=(connection_config_t&&) = default;
    ~connection_config_t() = default;
  };

}  // namespace sqlpp::postgresql
