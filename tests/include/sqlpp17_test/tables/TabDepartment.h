#pragma once

/*
Copyright (c) 2016 - 2019, Roland Bock
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

#include <sqlpp17/data_types.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/table.h>

namespace test
{
  struct TabDepartment : public ::sqlpp::spec_base
  {
    SQLPP_NAME_TAGS_FOR_SQL_AND_CPP(tab_department, tabDepartment);

    struct Id : public ::sqlpp::spec_base
    {
      SQLPP_NAME_TAGS_FOR_SQL_AND_CPP(id, id);
      using value_type = std::int64_t;
      static constexpr auto can_be_null = false;
      static constexpr auto has_default_value = false;
      static constexpr auto has_auto_increment = true;
    };

    struct Name : public ::sqlpp::spec_base
    {
      SQLPP_NAME_TAGS_FOR_SQL_AND_CPP(name, name);
      using value_type = ::sqlpp::fixchar<100>;
      static constexpr auto can_be_null = true;
      static constexpr auto has_default_value = false;
      static constexpr auto has_auto_increment = false;
    };

    struct Division : public ::sqlpp::spec_base
    {
      SQLPP_NAME_TAGS_FOR_SQL_AND_CPP(division, division);
      using value_type = ::sqlpp::varchar<255>;
      static constexpr auto can_be_null = false;
      static constexpr auto has_default_value = true;
      static constexpr auto default_value = std::string_view("engineering");
      static constexpr auto has_auto_increment = false;
    };

    using _columns = ::sqlpp::type_vector<Id, Name, Division>;

    using primary_key = sqlpp::type_vector<Id>;
  };

  inline constexpr auto tabDepartment = sqlpp::table_t<TabDepartment>{};

}  // namespace test
