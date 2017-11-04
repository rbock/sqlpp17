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
#include <optional>

namespace sqlpp
{
  template <typename Row, typename Handle>
  struct result_t
  {
    using _row_t = Row;
    using _handle_t = Handle;

    std::optional<_row_t> _row;
    std::unique_ptr<_handle_t> _handle;

  public:
    result_t() = default;

    result_t(std::unique_ptr<_handle_t>&& handle) : _handle(_handle)
    {
    }

    result_t(const result_t&) = delete;
    result_t(result_t&&) = default;
    result_t& operator=(const result_t&) = delete;
    result_t& operator=(result_t&&) = default;
    ~result_t() = default;

    class end_iterator;

    // Iterator
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

      reference operator*() const
      {
        return _result._row;
      }

      pointer operator->() const
      {
        return &_result._row;
      }

      bool operator==(const iterator& rhs) const
      {
        return false;
      }

      bool operator==(const end_iterator& rhs) const
      {
        return not _result._handle;
      }

      template <typename T>
      bool operator!=(const T& rhs) const
      {
        return not(operator==(rhs));
      }

      iterator& operator++()
      {
        get_next_row(_result._handle, _result._row);
        return *this;
      }

      iterator operator++(int)
      {
        auto previous_it = *this;
        ++(*this);
        return previous_it;
      }
    };

    // End iterator
    class end_iterator
    {
    public:
      using iterator_category = std::input_iterator_tag;
      using value_type = _row_t;
      using pointer = const _row_t*;
      using reference = const _row_t&;
      using difference_type = std::ptrdiff_t;

      reference operator*() const = delete;

      pointer operator->() const = delete;

      bool operator==(const iterator& rhs) const
      {
        return rhs == *this;
      }

      bool operator==(const end_iterator& rhs) const
      {
        return true;
      }

      template <typename T>
      bool operator!=(const T& rhs) const
      {
        return not(operator==(rhs));
      }

      iterator& operator++() = delete;

      iterator operator++(int) = delete;
    };

    iterator begin()
    {
      if (!_row)
      {
        _row.emplace();
        get_next_row(_handle, *_row);
      }
      return iterator(_handle, *_row);
    }

    iterator end()
    {
      return end_iterator();
    }

    bool empty() const
    {
      return begin() == end();
    }

    const _row_t& front() const
    {
      return *begin();
    }

    void pop_front()
    {
      ++(begin());
    }
  };

}  // namespace sqlpp
