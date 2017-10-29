#pragma once

/*
Copyright (c) 2016, Roland Bock
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

#include <type_traits>
#include <utility>

namespace sqlpp
{
  namespace detail
  {
    template <typename T>
    struct _base
    {
    };

    template <typename... Elements>
    struct _type_set
    {
    private:
      struct _impl : _base<Elements>...
      {
      };

    public:
      [[nodiscard]] static constexpr auto size()
      {
        return sizeof...(Elements);
      }

      [[nodiscard]] static constexpr auto empty()
      {
        return size() == 0;
      }

      template <typename T>
      [[nodiscard]] static constexpr auto count()
      {
        return std::is_base_of<_base<T>, _impl>::value;
      }

      template <typename T>
      [[nodiscard]] static constexpr auto insert()
      {
        return std::conditional_t<count<T>(), _type_set, _type_set<Elements..., T>>{};
      }

      template <typename... T>
      [[nodiscard]] constexpr auto is_disjoint_from(_type_set<T...>) const
      {
        return (true && ... && (!count<T>()));
      }

      template <typename T>
      [[nodiscard]] constexpr auto remove()
      {
        return *this - _type_set<T>{};
      }
    };

    template <typename... Ls, typename R>
    [[nodiscard]] constexpr auto operator<<(_type_set<Ls...> lhs, _base<R>)
    {
      return lhs.template insert<R>();
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator>=(_type_set<Ls...> lhs, _type_set<Rs...>)
    {
      return (true && ... && lhs.template count<Rs>());
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator<=(_type_set<Ls...> lhs, _type_set<Rs...> rhs)
    {
      return rhs >= lhs;
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator==(_type_set<Ls...> lhs, _type_set<Rs...> rhs)
    {
      return rhs <= lhs && lhs <= rhs;
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator!=(_type_set<Ls...> lhs, _type_set<Rs...> rhs)
    {
      return !(lhs == rhs);
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator|(_type_set<Ls...> lhs, _type_set<Rs...> rhs)
    {
      return (lhs << ... << _base<Rs>{});
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator&(_type_set<Ls...> lhs, _type_set<Rs...>)
    {
      return (_type_set<>{} | ... | std::conditional_t<lhs.template count<Rs>(), _type_set<Rs>, _type_set<>>{});
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator-(_type_set<Ls...>, _type_set<Rs...> rhs)
    {
      return (_type_set<>{} | ... | std::conditional_t<rhs.template count<Ls>(), _type_set<>, _type_set<Ls>>{});
    }

    template <typename... Ls, typename... Rs>
    [[nodiscard]] constexpr auto operator^(_type_set<Ls...> lhs, _type_set<Rs...> rhs)
    {
      return (lhs | rhs) - (rhs & rhs);
    }
  }

  template <typename... Ts>
  constexpr auto type_set()
  {
    return (detail::_type_set<>{} << ... << detail::_base<Ts>{});
  }

  template <typename T, typename... Ts>
  constexpr auto type_set(const T&, const Ts&...)
  {
    return (detail::_type_set<T>{} << ... << detail::_base<Ts>{});
  }

  template <template <typename> typename Condition, typename... Ts>
  constexpr auto type_set_if()
  {
    return (detail::_type_set<>{} | ... |
            std::conditional_t<Condition<Ts>::value, detail::_type_set<Ts>, detail::_type_set<>>{});
  }

  template <template <typename> typename Transform, typename... Ts>
  [[nodiscard]] constexpr auto transform(const detail::_type_set<Ts...>&)
  {
    return type_set<Transform<Ts>...>();
  }

  template <typename... Ts>
  using type_set_t = decltype(type_set<Ts...>());
}
