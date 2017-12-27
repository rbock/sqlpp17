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

#include <sqlpp17/postgresql/connection_pool.h>

namespace sqlpp::postgresql::detail
{
  auto circular_connection_buffer_t::increment(std::size_t& pos)
  {
    ++pos;
    if (pos == _data.size())
      pos = 0;
  }

  circular_connection_buffer_t::circular_connection_buffer_t(std::size_t capacity) : _data(capacity)
  {
    // This comment is just here because my version of clang-format barfs on this otherwise
  }

  [[nodiscard]] auto circular_connection_buffer_t::empty() const
  {
    return _size == 0;
  }

  [[nodiscard]] auto& circular_connection_buffer_t::front()
  {
    return _data[_tail];
  }

  auto circular_connection_buffer_t::pop_front() -> void
  {
    if ((_head != _tail) or not empty())
    {
      increment(_tail);
      --_size;
    }
  }

  auto circular_connection_buffer_t::push_back(detail::unique_connection_ptr t)
  {
    _data[_head] = std::move(t);
    if ((_head != _tail) or empty())
    {
      increment(_head);
      ++_size;
    }
    else  // (head == tail) and not empty()
    {
      increment(_head);
      _tail = _head;
    }
  }
}  // namespace sqlpp::postgresql::detail

namespace sqlpp::postgresql
{
  [[nodiscard]] __attribute__((no_sanitize("memory"))) auto connection_pool_t::get()
      -> ::sqlpp::postgresql::connection_t
  {
    const auto lock = std::scoped_lock{_mutex};

    auto handle = detail::unique_connection_ptr(std::move(_handles.front()));
    _handles.pop_front();

    // destroy dead connections
    if (handle != nullptr and PQstatus(handle.get()) != CONNECTION_OK)
    {
      handle.reset();
    }

    return handle ? ::sqlpp::postgresql::connection_t{_connection_config, std::move(handle), this}
                  : ::sqlpp::postgresql::connection_t{_connection_config, this};
  }

  auto connection_pool_t::put(detail::unique_connection_ptr handle) -> void
  {
    const auto lock = std::scoped_lock{_mutex};
    _handles.push_back(std::move(handle));
  }

}  // namespace sqlpp::postgresql
