#pragma once

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

#include <cfloat>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace sqlpp::test
{
  void compare(std::size_t index, float expected, float received)
  {
    const auto expectedString = (std::ostringstream{} << std::setprecision(5) << expected).str();
    const auto receivedString = (std::ostringstream{} << std::setprecision(5) << received).str();

    if (expectedString != receivedString)
    {
      std::cerr << "Float mismatch at index " << index << ": " << std::endl;
      std::cerr << "Expected: " << expectedString << " " << expected << std::endl;
      std::cerr << "Received: " << receivedString << " " << received << std::endl;
      throw std::runtime_error("unexpected float result");
    }
  }

  void compare(std::size_t index, double expected, double received)
  {
    const auto expectedString = (std::ostringstream{} << std::setprecision(10) << expected).str();
    const auto receivedString = (std::ostringstream{} << std::setprecision(10) << received).str();

    if (expectedString != receivedString)
    {
      std::cerr << "Double mismatch at index " << index << ": " << std::endl;
      std::cerr << "Expected: " << expectedString << " " << expected << std::endl;
      std::cerr << "Received: " << receivedString << " " << received << std::endl;
      throw std::runtime_error("unexpected double result");
    }
  }

  void compare(std::size_t index, int32_t expected, int32_t received)
  {
    if (expected != received)
    {
      std::cerr << "int32 mismatch at index " << index << ": " << std::endl;
      std::cerr << "Expected: " << expected << std::endl;
      std::cerr << "Received: " << received << std::endl;
      throw std::runtime_error("unexpected int32 result");
    }
  }

  void compare(std::size_t index, int64_t expected, int64_t received)
  {
    if (expected != received)
    {
      std::cerr << "int64 mismatch at index " << index << ": " << std::endl;
      std::cerr << "Expected: " << expected << std::endl;
      std::cerr << "Received: " << received << std::endl;
      throw std::runtime_error("unexpected int64 result");
    }
  }
}

