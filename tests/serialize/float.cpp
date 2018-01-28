/*
Copyright (c) 2018 - 2018, Roland Bock
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

#include <sqlpp17/to_sql_string.h>

#include <sqlpp17_test/mock_db.h>

#include "assert_equality.h"

using ::sqlpp::test::assert_equality;
using ::sqlpp::test::mock_context_t;

namespace
{
  template <typename T>
  auto test_nan(const T& t)
  {
    try
    {
      std::ignore = to_sql_string_c(mock_context_t{}, t);
      throw std::logic_error("to_sql_string should have thrown");
    }
    catch (const std::exception& e)
    {
      assert_equality("Serialization of NaN is not supported by this connector", std::string(e.what()));
    }
  }

  template <typename T>
  auto test_inf(const T& t)
  {
    try
    {
      std::ignore = to_sql_string_c(mock_context_t{}, t);
      throw std::logic_error("to_sql_string should have thrown");
    }
    catch (const std::exception& e)
    {
      assert_equality("Serialization of Infinity is not supported by this connector", std::string(e.what()));
    }
  }
}  // namespace

int main()
{
  try
  {
    assert_equality("0.1234567", to_sql_string_c(mock_context_t{}, 0.1234567890123456789f).substr(0, 9));
    assert_equality("0.123456789012345", to_sql_string_c(mock_context_t{}, 0.1234567890123456789).substr(0, 17));

    if constexpr (std::numeric_limits<float>::is_iec559)
    {
      test_nan(std::nanf(""));
      test_inf(std::numeric_limits<float>::infinity());
      test_inf(-std::numeric_limits<float>::infinity());
    }

    if constexpr (std::numeric_limits<double>::is_iec559)
    {
      test_nan(std::nan(""));
      test_inf(std::numeric_limits<double>::infinity());
      test_inf(-std::numeric_limits<double>::infinity());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
