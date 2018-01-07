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

#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/operator/equal_to.h>

SQLPP_CREATE_NAME_TAG(foo);
SQLPP_CREATE_NAME_TAG(bar);
SQLPP_CREATE_NAME_TAG(ping);
SQLPP_CREATE_NAME_TAG(pong);

using test::tabDepartment;
using test::tabEmpty;
using test::tabPerson;

// non-tables
static_assert(sqlpp::columns_of(7).empty());
static_assert(columns_of(tabPerson.id).empty());
static_assert(columns_of(tabPerson.id.as(tabPerson)).empty());

// raw tables
static_assert(columns_of(tabEmpty).empty());
static_assert(columns_of(tabPerson) ==
              type_set(tabPerson.id, tabPerson.isManager, tabPerson.name, tabPerson.address, tabPerson.language));
static_assert(columns_of(tabDepartment) == type_set(tabDepartment.id, tabDepartment.name));

// aliased tables
static_assert(columns_of(tabEmpty.as(foo)).empty());
static_assert(columns_of(tabPerson.as(foo)) == type_set(tabPerson.as(foo).id,
                                                        tabPerson.as(foo).isManager,
                                                        tabPerson.as(foo).name,
                                                        tabPerson.as(foo).address,
                                                        tabPerson.as(foo).language));
static_assert(columns_of(tabDepartment.as(foo)) == type_set(tabDepartment.as(foo).id, tabDepartment.as(foo).name));

// conditionless joins
constexpr auto colsOfDepartmentAndPerson = (columns_of(tabDepartment) | columns_of(tabPerson));
static_assert(columns_of(tabDepartment.join(tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.inner_join(tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.left_outer_join(tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.right_outer_join(tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.outer_join(tabPerson)) == colsOfDepartmentAndPerson);

// conditionless self joins
constexpr auto colsOfFooAndBar(columns_of(tabPerson.as(foo)) | columns_of(tabPerson.as(bar)));
static_assert(columns_of(tabPerson.as(foo).join(tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(tabPerson.as(foo).inner_join(tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(tabPerson.as(foo).left_outer_join(tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(tabPerson.as(foo).right_outer_join(tabPerson.as(bar))) == colsOfFooAndBar);
static_assert(columns_of(tabPerson.as(foo).outer_join(tabPerson.as(bar))) == colsOfFooAndBar);

// unconditional or cross joins
static_assert(columns_of(tabDepartment.cross_join(tabPerson)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.join(tabPerson).unconditionally()) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.inner_join(tabPerson).unconditionally()) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.left_outer_join(tabPerson).unconditionally()) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.right_outer_join(tabPerson).unconditionally()) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabDepartment.outer_join(tabPerson).unconditionally()) == colsOfDepartmentAndPerson);

// triple and quadruple joins
constexpr auto colsOfFooAndBarAndPing(columns_of(tabPerson.as(foo)) | columns_of(tabPerson.as(bar)) |
                                      columns_of(tabPerson.as(ping)));
constexpr auto colsOfFooAndBarAndPingAndPong(columns_of(tabPerson.as(foo)) | columns_of(tabPerson.as(bar)) |
                                             columns_of(tabPerson.as(ping)) | columns_of(tabPerson.as(pong)));
static_assert(columns_of(tabDepartment.cross_join(tabPerson).cross_join(tabEmpty)) == colsOfDepartmentAndPerson);
static_assert(columns_of(tabPerson.as(foo).cross_join(tabPerson.as(bar)).cross_join(tabPerson.as(ping))) ==
              colsOfFooAndBarAndPing);
static_assert(columns_of(tabPerson.as(foo)
                             .cross_join(tabPerson.as(bar))
                             .cross_join(tabPerson.as(ping))
                             .cross_join(tabPerson.as(pong))) == colsOfFooAndBarAndPingAndPong);
static_assert(columns_of(tabPerson.as(foo)
                             .cross_join(tabPerson.as(bar))
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
