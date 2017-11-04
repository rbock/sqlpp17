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

#include <sqlpp17/result.h>

namespace test
{
  class mock_handle
  {
  };

  template <typename Row>
  void get_next_result_row(std::unique_ptr<mock_handle>& handle, Row& row)
  {
    handle.reset();  // indicates that there are no more result rows to be read
  }

  class mock_db
  {
  public:
    template <typename Statement>
    [[nodiscard]] auto insert(const Statement& statement)
    {
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto update(const Statement& statement)
    {
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto erase(const Statement& statement)
    {
      return 0ull;
    }

    template <typename Statement>
    [[nodiscard]] auto select(const Statement& statement)
    {
      return ::sqlpp::result_t<typename Statement::_result_row_t, mock_handle>{std::make_unique<mock_handle>()};
    }

    template <typename Statement>
    [[nodiscard]] auto operator()(const Statement& statement)
    {
      // Need to do a final consistency check here
      return statement.run(*this);
    }
  };
}  // namespace test
