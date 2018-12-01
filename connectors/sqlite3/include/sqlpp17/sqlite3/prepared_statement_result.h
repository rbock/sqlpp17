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

#include <sqlpp17/result_row.h>

namespace sqlpp::sqlite3::detail
{
  enum class result_owns_statement : bool {};

  struct prepared_statement_cleanup_t
  {
    bool _owning;

    auto operator()(::sqlite3_stmt* handle) noexcept -> void
    {
      if (_owning and handle)
      {
        // This might fail, but throwing is not an option here
        sqlite3_finalize(handle);
      }
    }
  };
  using unique_prepared_statement_ptr = std::unique_ptr<::sqlite3_stmt, detail::prepared_statement_cleanup_t>;

  inline auto get_next_result_row(::sqlite3_stmt* stmt) -> bool
  {
    auto rc = sqlite3_step(stmt);

    switch (rc)
    {
      case SQLITE_ROW:
        return true;
      case SQLITE_DONE:
        return false;
      default:
        throw sqlpp::exception("Sqlite3 error: Unexpected return value for sqlite3_step(): " +
                               std::string(sqlite3_errstr(rc)));
    }
  }
}  // namespace sqlpp::sqlite3::detail

namespace sqlpp::sqlite3
{
  inline auto assign_field(sqlite3_stmt* stmt, bool& value, int index) -> void
  {
    value = sqlite3_column_int(stmt, index);
  }

  inline auto assign_field(sqlite3_stmt* stmt, std::int32_t& value, int index) -> void
  {
    value = sqlite3_column_int(stmt, index);
  }

  inline auto assign_field(sqlite3_stmt* stmt, std::int64_t& value, int index) -> void
  {
    value = sqlite3_column_int64(stmt, index);
  }

  inline auto assign_field(sqlite3_stmt* stmt, float& value, int index) -> void
  {
    // There is no column_float
    value = sqlite3_column_double(stmt, index);
  }

  inline auto assign_field(sqlite3_stmt* stmt, double& value, int index) -> void
  {
    value = sqlite3_column_double(stmt, index);
  }

  inline auto assign_field(sqlite3_stmt* stmt, std::string_view& value, int index) -> void
  {
    value = std::string_view{reinterpret_cast<const char*>(sqlite3_column_text(stmt, index)),
                             static_cast<std::size_t>(sqlite3_column_bytes(stmt, index))};
  }

  template <typename T>
  auto assign_field(sqlite3_stmt* stmt, std::optional<T>& value, int index) -> void
  {
    if (sqlite3_column_type(stmt, index) == SQLITE_NULL)
    {
      value.reset();
    }
    else
    {
      value = T{};
      assign_field(stmt, *value, index);
    }
  }

  template <typename... ColumnSpecs, unsigned... Is>
  auto assign_fields(sqlite3_stmt* stmt, result_row_t<ColumnSpecs...>& row, std::integer_sequence<unsigned, Is...>)
      -> void
  {
    (..., assign_field(stmt, static_cast<result_column_base<ColumnSpecs>&>(row)(), Is));
  }

  template <typename ResultRow>
  class prepared_statement_result_t
  {
    static_assert(wrong<ResultRow>, "ResultRow must be a result_row_t<...>");
  };

  template <typename... ColumnSpecs>
  class prepared_statement_result_t<result_row_t<ColumnSpecs...>>
  {
    detail::unique_prepared_statement_ptr _handle;

    result_row_t<ColumnSpecs...> _row;

  public:
    using row_type = decltype(_row);

    prepared_statement_result_t() = default;
    prepared_statement_result_t(detail::unique_prepared_statement_ptr&& handle)
        : _handle(std::move(handle))
    {
    }
    prepared_statement_result_t(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t(prepared_statement_result_t&& rhs) = default;
    prepared_statement_result_t& operator=(const prepared_statement_result_t&) = delete;
    prepared_statement_result_t& operator=(prepared_statement_result_t&&) = default;
    ~prepared_statement_result_t(){}

    auto get_next_row() -> void
    {
      if (detail::get_next_result_row(_handle.get()))
      {
        assign_fields(_handle.get(), _row, std::make_integer_sequence<unsigned, sizeof...(ColumnSpecs)>{});
      }
      else
      {
        reset();
      }
    }

    [[nodiscard]] auto& row() const
    {
      return _row;
    }

    [[nodiscard]] operator bool() const
    {
      return !!_handle;
    }

    [[nodiscard]] auto* get() const
    {
      return _handle.get();
    }

    auto reset() -> void
    {
      *this = {};
    }
  };

}  // namespace sqlpp::sqlite3

