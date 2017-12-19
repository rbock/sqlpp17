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

#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>
#include <sqlpp17/alias_provider.h>
#include <sqlpp17/operator/equal_to.h>

SQLPP_ALIAS_PROVIDER(foo);
SQLPP_ALIAS_PROVIDER(bar);
SQLPP_ALIAS_PROVIDER(ping);
SQLPP_ALIAS_PROVIDER(pong);

using test::tabDepartment;
using test::tabEmpty;
using test::tabPerson;

// non-tables
static_assert(sqlpp::columns_of(7).empty());
static_assert(columns_of(test::tabPerson.id).empty());
static_assert(columns_of(test::tabPerson.id.as(test::tabPerson)).empty());

// raw tables
static_assert(columns_of(test::tabEmpty).empty());
static_assert(columns_of(test::tabPerson) ==
              type_set(sqlpp::column_t<test::TabPerson_::_, test::TabPerson_::Id>{},
                       sqlpp::column_t<test::TabPerson_::_, test::TabPerson_::IsManager>{},
                       sqlpp::column_t<test::TabPerson_::_, test::TabPerson_::Name>{},
                       sqlpp::column_t<test::TabPerson_::_, test::TabPerson_::Address>{}));
static_assert(columns_of(test::tabDepartment) ==
              type_set(sqlpp::column_t<test::TabDepartment_::_, test::TabDepartment_::Id>{},
                       sqlpp::column_t<test::TabDepartment_::_, test::TabDepartment_::Name>{}));

// aliased tables
static_assert(columns_of(test::tabEmpty.as(foo)).empty());
static_assert(columns_of(test::tabPerson.as(foo)) == type_set(sqlpp::column_t<foo_t, test::TabPerson_::Id>{},
                                                              sqlpp::column_t<foo_t, test::TabPerson_::IsManager>{},
                                                              sqlpp::column_t<foo_t, test::TabPerson_::Name>{},
                                                              sqlpp::column_t<foo_t, test::TabPerson_::Address>{}));
static_assert(columns_of(test::tabDepartment.as(foo)) ==
              type_set(sqlpp::column_t<foo_t, test::TabDepartment_::Id>{},
                       sqlpp::column_t<foo_t, test::TabDepartment_::Name>{}));

// conditionless joins
constexpr auto colsOfDepartmentAndPerson = (columns_of(test::tabDepartment) | columns_of(test::tabPerson));
static_assert(columns_of(test::tabDepartment.join(test::tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.inner_join(test::tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.left_outer_join(test::tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.right_outer_join(test::tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.outer_join(test::tabPerson)) == colsOfDepartmentAndPerson);

// conditionless self joins
constexpr auto colsOfFooAndBar(columns_of(test::tabPerson.as(foo)) | columns_of(test::tabPerson.as(bar)));
static_assert(columns_of(test::tabPerson.as(foo).join(test::tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(test::tabPerson.as(foo).inner_join(test::tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(test::tabPerson.as(foo).left_outer_join(test::tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(test::tabPerson.as(foo).right_outer_join(test::tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(test::tabPerson.as(foo).outer_join(test::tabPerson.as(bar))) == colsOfFooAndBar);

// unconditional or cross joins
static_assert(columns_of(test::tabDepartment.cross_join(test::tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.join(test::tabPerson).unconditionally()) == colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.inner_join(test::tabPerson).unconditionally()) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.left_outer_join(test::tabPerson).unconditionally()) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.right_outer_join(test::tabPerson).unconditionally()) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabDepartment.outer_join(test::tabPerson).unconditionally()) ==
              colsOfDepartmentAndPerson);

// triple and quadruple joins
constexpr auto colsOfFooAndBarAndPing(columns_of(test::tabPerson.as(foo)) | columns_of(test::tabPerson.as(bar)) |
                                      columns_of(tabPerson.as(ping)));
constexpr auto colsOfFooAndBarAndPingAndPong(columns_of(test::tabPerson.as(foo)) | columns_of(test::tabPerson.as(bar)) |
                                             columns_of(tabPerson.as(ping)) | columns_of(tabPerson.as(pong)));
static_assert(columns_of(test::tabDepartment.cross_join(test::tabPerson).cross_join(tabEmpty)) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(test::tabPerson.as(foo).cross_join(test::tabPerson.as(bar)).cross_join(tabPerson.as(ping))) ==
              colsOfFooAndBarAndPing);
static_assert(columns_of(test::tabPerson.as(foo)
                             .cross_join(test::tabPerson.as(bar))
                             .cross_join(tabPerson.as(ping))
                             .cross_join(tabPerson.as(pong))) == colsOfFooAndBarAndPingAndPong);
static_assert(columns_of(test::tabPerson.as(foo)
                             .cross_join(test::tabPerson.as(bar))
                             .cross_join((tabPerson.as(ping)).cross_join(tabPerson.as(pong)))) ==
              colsOfFooAndBarAndPingAndPong);

// conditional joins
static_assert(columns_of(tabDepartment.join(tabPerson).on(tabDepartment.id == tabPerson.id)) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.inner_join(tabPerson).on(tabDepartment.id == tabPerson.id)) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.left_outer_join(tabPerson).on(tabDepartment.id == tabPerson.id)) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.right_outer_join(tabPerson).on(tabDepartment.id == tabPerson.id)) ==
              colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.outer_join(tabPerson).on(tabDepartment.id == tabPerson.id)) ==
              colsOfDepartmentAndPerson);

int main()
{
}
