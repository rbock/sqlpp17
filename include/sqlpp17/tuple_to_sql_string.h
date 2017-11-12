#pragma once

/*
Copyright (c) 2016 - 2017, Roland Bock
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

#include <string>
#include <tuple>

#include <sqlpp17/to_sql_string.h>

namespace sqlpp ::detail
{
  template <typename DbConnection, typename... Ts, std::size_t... Is>
  [[nodiscard]] auto tuple_to_string_impl(const DbConnection& connection,
                                          const std::string& separator,
                                          const std::tuple<Ts...>& t,
                                          std::integer_sequence<std::size_t, Is...>)
  {
    return (std::string{} + ... + ((Is ? separator : "") + to_sql_string(connection, std::get<Is>(t))));
  }
}  // namespace sqlpp::detail

namespace sqlpp
{
  template <typename DbConnection, typename... Ts>
  [[nodiscard]] auto tuple_to_string(const DbConnection& connection,
                                     const std::string& separator,
                                     const std::tuple<Ts...>& t)
  {
    return detail::tuple_to_string_impl(connection, separator, t, std::make_index_sequence<sizeof...(Ts)>());
  }
}  // namespace sqlpp
