#pragma once

/*
Copyright (c) 2016 - 2018, Roland Bock
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

#include <string_view>

#include <sqlpp17/type_traits.h>

#define SQLPP_NAME_TAG_GUTS(SQL_NAME, CPP_NAME)             \
  /* would like to make this a function, but clang crashes, \
     see https://bugs.llvm.org/show_bug.cgi?id=35829)*/     \
  static constexpr auto name = std::string_view{#SQL_NAME}; \
  template <typename T>                                     \
  struct _sqlpp_member_base                                 \
  {                                                         \
    T CPP_NAME = {};                                        \
    T& operator()()                                         \
    {                                                       \
      return CPP_NAME;                                      \
    }                                                       \
    const T& operator()() const                             \
    {                                                       \
      return CPP_NAME;                                      \
    }                                                       \
  };

#define SQLPP_NAME_TAGS_FOR_SQL_AND_CPP(SQL_NAME, CPP_NAME) \
  struct _sqlpp_name_tag : public ::sqlpp::name_tag_base    \
  {                                                         \
    SQLPP_NAME_TAG_GUTS(SQL_NAME, CPP_NAME)                 \
  }

#define SQLPP_CREATE_NAME_TAG(NAME)                                \
  struct sqlpp_name_tag_for_##NAME : public ::sqlpp::name_tag_base \
  {                                                                \
    SQLPP_NAME_TAG_GUTS(NAME, NAME)                                \
  };                                                               \
  constexpr auto NAME = sqlpp_name_tag_for_##NAME                  \
  {                                                                \
  }
