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

#include <optional>
#include <string>

#include <sqlpp17/table.h>

namespace test
{
  namespace TabPerson_
  {
    struct Id
    {
      SQLPP_ALIAS(id)
      using value_type = std::int64_t;
      static constexpr auto can_be_null = false;
      static constexpr auto default_value = ::sqlpp::auto_increment_t{};
    };

    struct IsManager
    {
      SQLPP_ALIAS(is_manager)
      using value_type = bool;
      static constexpr auto can_be_null = false;
      static constexpr auto default_value = ::sqlpp::none_t{};
    };

    struct Name
    {
      SQLPP_ALIAS(name)
      using value_type = std::string_view;
      static constexpr auto can_be_null = false;
      static constexpr auto default_value = ::sqlpp::none_t{};
    };

    struct Address
    {
      SQLPP_ALIAS(address)
      using value_type = std::optional<std::string_view>;
      static constexpr auto can_be_null = false;
      static constexpr char default_value[] = "";
    };

    struct _
    {
      SQLPP_ALIAS(tab_person)
      using primary_key = Id;
    };
  }  // namespace TabPerson_

  constexpr auto tabPerson =
      sqlpp::table_t<TabPerson_::_, TabPerson_::Id, TabPerson_::IsManager, TabPerson_::Name, TabPerson_::Address>{};
}  // namespace test
