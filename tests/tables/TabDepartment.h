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

#include <cstdint>

#include <sqlpp17/table.h>

namespace test
{
  namespace TabDepartment_
  {
    struct Id
    {
      struct _alias_t
      {
        static constexpr char name[] = "id";
        template <typename T>
        struct _member_t
        {
          T id;
        };
      };
      using value_type = std::int64_t;
      static constexpr auto tags = sqlpp::tag::must_not_insert | sqlpp::tag::must_not_update | sqlpp::tag::has_default;
    };

    struct _
    {
      struct _alias_t
      {
        static constexpr char name[] = "tab_department";
        template <typename T>
        struct _member_t
        {
          T tabDepartment;
        };
      };
    };
  }  // namespace TabDepartment_

  inline constexpr auto tabDepartment = sqlpp::table_t<TabDepartment_::_, TabDepartment_::Id>{};
}  // namespace test
