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

#include <iterator>
#include <memory>

#include <sqlpp17/core/type_traits.h>

namespace sqlpp
{
  class result_end_t
  {
  };

  template <typename ResultHandle>
  struct result_t
  {
    using _row_t = typename ResultHandle::row_type;
    ResultHandle _handle;

  public:
    result_t() = default;

    result_t(ResultHandle&& handle) : _handle(std::move(handle))
    {
    }

    result_t(const result_t&) = delete;
    result_t(result_t&&) = default;
    result_t& operator=(const result_t&) = delete;
    result_t& operator=(result_t&&) = default;
    ~result_t() = default;

    class iterator
    {
      result_t& _result;

    public:
      using iterator_category = std::input_iterator_tag;
      using value_type = _row_t;
      using pointer = const _row_t*;
      using reference = const _row_t&;
      using difference_type = std::ptrdiff_t;

      iterator(result_t& result) : _result(result)
      {
      }

      [[nodiscard]] auto operator*() const -> reference
      {
        return _result._handle.row();
      }

      [[nodiscard]] auto operator->() const -> pointer
      {
        return &_result._handle.row();
      }

      [[nodiscard]] auto operator==(const iterator& rhs) const -> bool
      {
        return false;
      }

      [[nodiscard]] auto operator==(const result_end_t& rhs) const -> bool
      {
        return not _result._handle;
      }

      template <typename T>
      auto operator!=(const T& rhs) const -> bool
      {
        return not(operator==(rhs));
      }

      auto operator++() -> iterator&
      {
        _result._handle.get_next_row();
        return *this;
      }

      auto operator++(int) -> iterator
      {
        auto previous_it = *this;
        ++(*this);
        return previous_it;
      }
    };

    [[nodiscard]] auto begin() -> iterator
    {
      _handle.get_next_row();
      return {*this};
    }

    [[nodiscard]] constexpr auto end() const -> result_end_t
    {
      return {};
    }

    [[nodiscard]] auto empty() -> bool
    {
      return begin() == end();
    }

    [[nodiscard]] auto front() -> const _row_t&
    {
      return *begin();
    }

    auto pop_front() -> void
    {
      ++(begin());
    }
  };

}  // namespace sqlpp
