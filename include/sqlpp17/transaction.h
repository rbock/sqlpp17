#pragma once

/*
Copyright (c) 2018, Roland Bock
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

#include <utility>

namespace sqlpp
{
  template <typename Connection>
  class transaction_t
  {
    static_assert(noexcept(std::declval<Connection>().destroy_transaction()));

    Connection& _connection;
    bool _committed = false;

  public:
    transaction_t(Connection& connection) : _connection(connection)
    {
      _connection.start_transaction();
    }

    transaction_t(const transaction_t&) = delete;

    transaction_t(transaction_t&& source):
      _connection(source._connection),
      _committed(source._committed)
    {
      source._committed = true;
    }

    transaction_t& operator=(const transaction_t&) = delete;
    transaction_t& operator=(transaction_t&&) = delete;  // See destructor

    ~transaction_t()
    {
      if (not _committed)
      {
        _connection.destroy_transaction();
      }
    }

    void commit()
    {
      if (not _committed)
      {
        _committed = true;
        _connection.commit();
      }
    }

    void rollback()
    {
      if (not _committed)
      {
        _committed = true;
        _connection.rollback();
      }
    }
  };

  template <typename Connection>
  auto start_transaction(Connection& connection)
  {
    return transaction_t{connection};
  }

}  // namespace sqlpp
