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

#include <sqlpp17/algorithm.h>
#include <sqlpp17/bad_statement.h>
#include <sqlpp17/clause_fwd.h>
#include <sqlpp17/detail/statement_constructor_arg.h>
#include <sqlpp17/result.h>
#include <sqlpp17/succeeded.h>
#include <sqlpp17/type_traits.h>
#include <sqlpp17/wrapped_static_assert.h>

namespace sqlpp
{
  template <typename ResultBase, typename Handle>
  class prepared_statement_t
  {
    Handle _handle;

  public:
    prepared_statement_t(const ResultBase& /*unused*/, Handle&& handle) : _handle(std::move(handle))
    {
    }

    auto run()
    {
      if constexpr (has_result_row_v<ResultBase>)
      {
        using _result_row_t = typename ResultBase::_result_row_t;
        using _result_handle_t = decltype(_handle.run());

        return ::sqlpp::result_t<_result_row_t, _result_handle_t>{_handle.run()};
      }
      else
      {
        return _handle.run();
      }
    }
  };

}  // namespace sqlpp
