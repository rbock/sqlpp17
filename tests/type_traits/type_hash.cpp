/*
Copyright (c) 2018, Roland Bock
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

#include <string>
#include <string_view>
#include <utility>

#include <sqlpp17/type_hash.h>

#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabEmpty.h>
#include <sqlpp17_test/tables/TabPerson.h>

template <typename T1, typename T2>
auto compare_two_hashes()
{
  if constexpr (std::is_same_v<T1, T2>)
  {
    static_assert(::sqlpp::type_hash<T1>() == ::sqlpp::type_hash<T2>());
  }
  else
    static_assert(::sqlpp::type_hash<T1>() != ::sqlpp::type_hash<T2>());
}

template <typename T, typename... Rest>
auto compare_one_to_all_hashes()
{
  (compare_two_hashes<T, Rest>(), ...);
}

template <typename... Ts>
void compare_all_hashes()
{
  (compare_one_to_all_hashes<Ts, Ts...>(), ...);
}

template <typename... Ts>
void compare_all_hashes(Ts...)
{
  (compare_one_to_all_hashes<Ts, Ts...>(), ...);
}

template <typename...>
struct X;

int main()
{
  compare_all_hashes<bool, char, int, float, long, std::string, std::string_view, X<>>();
  compare_all_hashes(test::tabDepartment, test::tabEmpty, test::tabPerson);
}

