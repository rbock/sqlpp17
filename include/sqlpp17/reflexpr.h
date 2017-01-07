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

#ifdef __cpp_reflection

#include <reflexpr>

namespace std::meta
{
  inline namespace func
  {
    template <typename T>
    struct meta_object
    {
      static_assert(std::is_metaobject_v<T>);
      using type = T;
    };

    namespace detail
    {
      template <typename... T>
      using meta_tuple = std::tuple<meta_object<T>...>;
    }

    template <typename T>
    constexpr auto reflect(const T&)
    {
      return std::meta::func::meta_object<std::meta::get_aliased_m<reflexpr(T)>>{};
    }

    template <typename T>
    constexpr auto get_base_name(const meta_object<T>&)
    {
      return std::meta::get_base_name_v<T>;
    }

    template <typename T>
    constexpr auto get_data_members(const meta_object<T>&)
    {
      return std::meta::unpack_sequence_t<std::meta::get_data_members_m<T>, detail::meta_tuple>{};
    }

    template <typename T>
    constexpr auto get_inheritances(const meta_object<T>&)
    {
      return std::meta::unpack_sequence_t<std::meta::get_base_classes_m<T>, detail::meta_tuple>{};
    }

    template <typename T>
    constexpr auto get_class(const meta_object<T>&)
    {
      return meta_object<std::meta::get_base_class_m<T>>{};
    }
  }
}

#endif
