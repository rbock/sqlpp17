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

#include <sqlpp17/clause/insert_into.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/operator.h>
#include <sqlpp17/parameter.h>

#include <connections/mock_db.h>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

using test::tabDepartment;
using test::tabPerson;

static_assert(::sqlpp::required_insert_columns_of_f(tabDepartment) == ::sqlpp::type_set());
static_assert(::sqlpp::required_insert_columns_of_f(tabPerson) ==
              ::sqlpp::type_set(tabPerson.isManager, tabPerson.name));

SQLPP_CREATE_NAME_TAG(pName);
SQLPP_CREATE_NAME_TAG(pIsManager);
SQLPP_CREATE_NAME_TAG(pAddress);

int main()
{
  auto db = ::sqlpp::test::mock_db{};

  {
    auto s = db.prepare(insert_into(tabDepartment).default_values());
    [[maybe_unused]] const auto id = execute(s);
  }

  {
    auto s = db.prepare(insert_into(tabPerson).set(tabPerson.isManager = true,
                                                   tabPerson.name = ::sqlpp::parameter<std::string>(pName)));
    s.pName = "Herb";
    [[maybe_unused]] const auto id = execute(s);
  }

  {
    auto s = db.prepare(insert_into(tabPerson).set(
        tabPerson.isManager = ::sqlpp::parameter<bool>(pIsManager),
        tabPerson.name = ::sqlpp::parameter<std::string>(pName),
        tabPerson.address = ::sqlpp::parameter<::std::optional<std::string>>(pAddress), tabPerson.language = "C++"));
    s.pIsManager = true;
    s.pName = "Herb";
    s.pAddress = "Somewhere";
    s.pAddress = std::nullopt;
    s.pAddress.reset();

    [[maybe_unused]] const auto id = execute(s);
  }
}
