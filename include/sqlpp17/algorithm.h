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
#include <sqlpp17/type_vector.h>
#include <sqlpp17/wrong.h>

namespace sqlpp
{
  namespace algorithm
  {
    template <typename Container, typename OldType, typename NewType>
    struct replace
    {
      static_assert(wrong<Container>, "replace must be used with a container type");
    };

    template <template <typename...> class Container, typename... Ts, typename OldType, typename NewType>
    struct replace<Container<Ts...>, OldType, NewType>
    {
      using type = Container<std::conditional_t<std::is_same<Ts, OldType>::value, NewType, Ts>...>;
    };

    template <typename Container, typename OldType, typename NewType>
    using replace_t = typename replace<Container, OldType, NewType>::type;

    template <typename Container, template <typename...> class Target>
    struct copy
    {
      static_assert(wrong<Container>, "copy must be used with a container type");
    };

    template <template <typename...> class Container, typename... Ts, template <typename...> class Target>
    struct copy<Container<Ts...>, Target>
    {
      using type = Target<Ts...>;
    };

    template <typename Container, template <typename...> class Target>
    using copy_t = typename copy<Container, Target>::type;

#warning : Add a copy_if.
  }
}
