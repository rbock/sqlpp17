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

#define SQLPP_WRAPPED_STATIC_ASSERT_DISABLE true

#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>
#include <tables/TabDepartment.h>

#include <sqlpp17/from.h>

namespace sqlpp
{
  template <typename Assert, typename T>
  struct is_bad_statement : public std::false_type
  {
  };

  template <typename Assert>
  struct is_bad_statement<Assert, bad_statement_t<failed<Assert>>> : public std::true_type
  {
  };
}

int main(int, char* [])
{
  constexpr auto s = sqlpp::statement<void, sqlpp::no_from_t>{};

  static_assert(sqlpp::is_bad_statement<sqlpp::assert_from_arg_is_not_conditionless_join,
                                        decltype(s.from(test::tabPerson.join(test::tabDepartment)))>::value);
  static_assert(sqlpp::is_bad_statement<sqlpp::assert_from_arg_is_table, decltype(s.from(1))>::value);

  static_assert(sqlpp::is_bad_statement<sqlpp::assert_from_arg_is_table, decltype(sqlpp::from(1))>::value);
}
