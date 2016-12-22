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

#include <sqlpp17/table.h>
#include <sqlpp17/data_type.h>

namespace test
{
  namespace TabPerson_
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
      using value_type = sqlpp::integral_t;
      static constexpr auto tags = sqlpp::tag::must_not_insert | sqlpp::tag::must_not_update | sqlpp::tag::has_default;
    };

    struct IsManager
    {
      struct _alias_t
      {
        static constexpr char name[] = "is_manager";
        template <typename T>
        struct _member_t
        {
          T isManager;
        };
      };
      using value_type = sqlpp::boolean_t;
      static constexpr auto tags = 0;
    };

    struct Name
    {
      struct _alias_t
      {
        static constexpr char name[] = "name";
        template <typename T>
        struct _member_t
        {
          T name;
        };
      };
      using value_type = sqlpp::text_t;
      static constexpr auto tags = sqlpp::tag::can_be_null | sqlpp::tag::null_is_trivial_value;
    };

    struct Address
    {
      struct _alias_t
      {
        static constexpr char name[] = "address";
        template <typename T>
        struct _member_t
        {
          T address;
        };
      };
      using value_type = sqlpp::text_t;
      static constexpr auto tags = sqlpp::tag::can_be_null;
    };

    struct _
    {
      struct _alias_t
      {
        static constexpr char name[] = "tab_person";
        template <typename T>
        struct _member_t
        {
          T tabPerson;
        };
      };
    };
  }

  constexpr auto tabPerson =
      sqlpp::table_t<TabPerson_::_, TabPerson_::Id, TabPerson_::IsManager, TabPerson_::Name, TabPerson_::Address>{};
}
