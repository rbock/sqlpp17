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

#include <type_traits>
#include <sqlpp17/type_vector.h>
#include <sqlpp17/wrong.h>

namespace sqlpp::mysql::detail
{
  auto get_handle(const connection_config& config) -> std::unique_ptr<MYSQL, void (*)(MYSQL*)>;
}

namespace sqlpp::mysql
{
  class connection
  {
  public:
    connection() = delete;

    connection(const connection_config& config) : _handle(detail::get_handle(config))
    {
    }

    template <typename Destructor>
    connection(unique_ptr<MYSQL, Destructor>&& handle) : _handle(std::move(handle))
    {
    }

    connection(const connection&) = delete;
    connection(connection&&) = default;
    connection& operator=(const connection&) = delete;
    connection& operator=(connection&&) = default;
    ~connection() = default;

    template <typename Statement>
    auto operator()(const Statement& statement)
    {
      // Need to do a final consistency check here
      return statement.run(*this);
    }

  private:
    template <typename Statement>
    auto insert(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    auto update(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    auto erase(const Statement& statement)
    {
#warning implement
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
#warning implement
      return ::sqlpp::result_t<typename Statement::_result_row_t, mock_handle>{std::make_unique<mock_handle>()};
    }

    std::unique_ptr<MYSQL, void (*)(MYSQL*)> _handle;
  };

}  // namespace sqlpp::mysql
