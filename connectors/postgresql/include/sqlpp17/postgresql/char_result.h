#pragma once

/*
Copyright (c) 2017 - 2018, Roland Bock
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

#include <sqlpp17/result_row.h>

#include <libpq-fe.h>

namespace sqlpp::postgresql::detail
{
  struct result_cleanup_t
  {
    auto operator()(PGresult* handle) const -> void
    {
      if (handle)
        PQclear(handle);
    }
  };
  using unique_result_ptr = std::unique_ptr<PGresult, detail::result_cleanup_t>;
}  // namespace sqlpp::postgresql::detail

namespace sqlpp::postgresql
{
  inline auto read_field(PGresult* result, int row_index, std::int32_t& value, int index) -> void
  {
    value = std::strtol(PQgetvalue(result, row_index, index), nullptr, 10);
  }

  inline auto read_field(PGresult* result, int row_index, std::int64_t& value, int index) -> void
  {
    value = std::strtoll(PQgetvalue(result, row_index, index), nullptr, 10);
  }

  inline auto read_field(PGresult* result, int row_index, float& value, int index) -> void
  {
    value = std::strtof(PQgetvalue(result, row_index, index), nullptr);
  }

  inline auto read_field(PGresult* result, int row_index, double& value, int index) -> void
  {
    value = std::strtod(PQgetvalue(result, row_index, index), nullptr);
  }

  inline auto read_field(PGresult* result, int row_index, std::string_view& value, int index) -> void
  {
    value = std::string_view(PQgetvalue(result, row_index, index),
                             PQgetlength(result, row_index, index));
  }

  inline auto read_field(PGresult* result, int row_index, std::optional<std::string_view>& value, int index) -> void
  {
    value = PQgetisnull(result, row_index, index)
                ? std::nullopt
                : std::optional<std::string_view>{
                      std::string_view(PQgetvalue(result, row_index, index),
                                       PQgetlength(result, row_index, index))};
  }

  template <typename... ColumnSpecs>
  auto read_fields(PGresult* result, int row_index, result_row_t<ColumnSpecs...>& row) -> void
  {
    std::size_t index = 0;
    (..., (read_field(result, row_index, static_cast<result_column_base<ColumnSpecs>&>(row)(), ++index)));
  }

  template<typename ResultRow>
  class char_result_t
  {
    static_assert(wrong<ResultRow>, "ResultRow must be a result_row_t<...>");
  };

  template <typename... ColumnSpecs>
  class char_result_t<result_row_t<ColumnSpecs...>>
  {
    detail::unique_result_ptr _handle;
    int _row_index = -1;
    int _row_count;

    result_row_t<ColumnSpecs...> _row;

  public:
    using row_type = decltype(_row);

    char_result_t() = default;
    char_result_t(detail::unique_result_ptr handle)
        : _handle(std::move(handle))
    {
      _row_count = PQntuples(_handle.get());
    }

    char_result_t(const char_result_t&) = delete;
    char_result_t(char_result_t&& rhs) = default;
    char_result_t& operator=(const char_result_t&) = delete;
    char_result_t& operator=(char_result_t&&) = default;
    ~char_result_t() = default;

    auto get_next_row() -> void
    {
      ++_row_index;
      if (_row_index < get_row_count())
      {
        read_fields(_handle.get(), _row_index, _row);
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

    auto* get() const
    {
      return _handle.get();
    }

    auto get_row_count() const
    {
      return _row_count;
    }

    auto reset() -> void
    {
      *this = char_result_t{};
    }
  };

}  // namespace sqlpp::postgresql

