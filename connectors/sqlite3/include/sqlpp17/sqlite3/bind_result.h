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

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

#include <sqlpp17/sqlite3/prepared_statement.h>

namespace sqlpp::sqlite3
{
  class bind_result_t;
  class prepared_statement_t;
}  // namespace sqlpp::sqlite3

namespace sqlpp::sqlite3::detail
{
  using maybe_owning_stmt_ptr = std::unique_ptr<::sqlite3_stmt, void (*)(::sqlite3_stmt*)>;

  auto get_next_result_row(bind_result_t& result) -> bool;
}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  class bind_result_t
  {
    detail::maybe_owning_stmt_ptr _handle;
    std::function<void(std::string_view)> _debug;

    friend auto detail::get_next_result_row(bind_result_t& result) -> bool;
    template <typename Row>
    friend auto get_next_result_row(bind_result_t& result, Row& row) -> void;

  public:
    bind_result_t() = default;
    bind_result_t(detail::maybe_owning_stmt_ptr handle, std::function<void(std::string_view)> debug)
        : _handle(std::move(handle)), _debug(debug)
    {
    }
    bind_result_t(const bind_result_t&) = delete;
    bind_result_t(bind_result_t&& rhs) = default;
    bind_result_t& operator=(const bind_result_t&) = delete;
    bind_result_t& operator=(bind_result_t&&) = default;
    ~bind_result_t() = default;

    [[nodiscard]] operator bool() const
    {
      return !!_handle;
    }

    [[nodiscard]] auto* get() const
    {
      return _handle.get();
    }

    [[nodiscard]] auto debug() const
    {
      return _debug;
    }

    auto invalidate()
    {
      _handle.reset();
    }
  };

  template <typename Row>
  auto get_next_result_row(bind_result_t& result, Row& row) -> void
  {
    if (detail::get_next_result_row(result))
    {
      row.post_bind(result);
    }
    else
    {
      result.invalidate();
    }
  }

  auto post_bind_field(bind_result_t& result, std::int64_t& value, std::size_t index) -> void;

  auto post_bind_field(bind_result_t& result, std::optional<std::string_view>& value, std::size_t index) -> void;
}  // namespace sqlpp::sqlite3
