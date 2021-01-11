/*
Copyright (c) 2016 - 2017, Roland Bock
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

#include <iostream>

#include <sqlpp17/core/to_sql_string.h>

int main()
{
  auto context = 0;

  std::cout << sqlpp::to_sql_string_c(context, true) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, false) << std::endl;

  std::cout << sqlpp::to_sql_string_c(context, 1234567890) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, 42) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, 0) << std::endl;

  std::cout << sqlpp::to_sql_string_c(context, 12345678901234567890ull) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, 42ull) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, 0ull) << std::endl;

  std::cout << sqlpp::to_sql_string_c(context, 1234567890.1234567890) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, -1234567890.1234567890) << std::endl;

  std::cout << sqlpp::to_sql_string_c(context, 'c') << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, "char*") << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, std::string_view("string_view")) << std::endl;
  std::cout << sqlpp::to_sql_string_c(context, std::string("string")) << std::endl;

#warning : need to test results
}
