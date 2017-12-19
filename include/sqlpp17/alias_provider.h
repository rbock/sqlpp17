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

#define SQLPP_ALIAS2(SQL_NAME, CPP_NAME)      \
  struct _alias_t                             \
  {                                           \
    static constexpr char name[] = #SQL_NAME; \
    template <typename T>                     \
    struct _member_t                          \
    {                                         \
      T CPP_NAME;                             \
      T& operator()()                         \
      {                                       \
        return CPP_NAME;                      \
      }                                       \
      const T& operator()() const             \
      {                                       \
        return CPP_NAME;                      \
      }                                       \
    };                                        \
  };

#define SQLPP_ALIAS(NAME) SQLPP_ALIAS2(NAME, NAME)

#define SQLPP_ALIAS_PROVIDER(NAME) \
  struct NAME##_t                  \
  {                                \
    SQLPP_ALIAS(NAME)              \
  };                               \
  constexpr auto NAME = NAME##_t   \
  {                                \
  }
