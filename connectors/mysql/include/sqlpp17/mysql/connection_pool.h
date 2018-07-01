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

#include <mutex>

#include <sqlpp17/mysql/connection.h>

namespace sqlpp::mysql
{
  class connection_t;
}

namespace sqlpp::mysql::detail
{
  class circular_connection_buffer_t
  {
    std::vector<detail::unique_connection_ptr> _data;
    std::size_t _head = 0;
    std::size_t _tail = 0;
    std::size_t _size = 0;

    auto increment(std::size_t& pos)
    {
      ++pos;
      if (pos == _data.size())
        pos = 0;
    }

  public:
    circular_connection_buffer_t(std::size_t capacity) : _data(capacity)
    {
    }

    [[nodiscard]] auto empty() const
    {
      return _size == 0;
    }

    [[nodiscard]] auto& front()
    {
      return _data[_tail];
    }

    auto pop_front() -> void
    {
      if ((_head != _tail) or not empty())
      {
        increment(_tail);
        --_size;
      }
    }

    auto push_back(detail::unique_connection_ptr t)
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
  };
}  // namespace sqlpp::mysql::detail

namespace sqlpp::mysql
{
  template <debug Debug>
  class connection_pool_t
  {
    connection_config_t _connection_config;
    detail::circular_connection_buffer_t _handles;
    std::mutex _mutex;

    friend class ::sqlpp::mysql::connection_t<connection_pool_t, Debug>;

  public:
    connection_pool_t() = delete;
    connection_pool_t(std::size_t capacity, connection_config_t connection_config)
        : _connection_config(std::move(connection_config)), _handles(capacity)
    {
    }
    connection_pool_t(const connection_pool_t&) = delete;
    connection_pool_t(connection_pool_t&&) = default;
    connection_pool_t& operator=(const connection_pool_t&) = delete;
    connection_pool_t& operator=(connection_pool_t&&) = default;
    ~connection_pool_t() = default;

    [[nodiscard]] __attribute__((no_sanitize("memory"))) auto get() -> ::sqlpp::mysql::connection_t
    {
      detail::thread_init();

      const auto lock = std::scoped_lock{_mutex};

      auto handle = detail::unique_connection_ptr(std::move(_handles.front()));
      _handles.pop_front();

      // destroy dead connections
      if (handle and mysql_ping(handle.get()) != 0)
      {
        handle.reset();
      }

      return handle ? ::sqlpp::mysql::connection_t{_connection_config, std::move(handle), this}
                    : ::sqlpp::mysql::connection_t{_connection_config, this};
    }

  private:
    auto put(detail::unique_connection_ptr handle) -> void
    {
      const auto lock = std::scoped_lock{_mutex};
      _handles.push_back(std::move(handle));
    }
  };

}  // namespace sqlpp::mysql
