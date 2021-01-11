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

#include <core_test/tables/TabDepartment.h>
#include <core_test/tables/TabEmpty.h>
#include <core_test/tables/TabPerson.h>
#include <sqlpp17/core/name_tag.h>

SQLPP_CREATE_NAME_TAG(foo);

// non-tables
static_assert(not sqlpp::is_table(7));
static_assert(not sqlpp::is_table(test::tabPerson.id));
static_assert(not sqlpp::is_table(test::tabPerson.id.as(test::tabPerson)));

// unconditional joins
static_assert(not sqlpp::is_table(test::tabEmpty.join(test::tabPerson)));
static_assert(not sqlpp::is_table(test::tabEmpty.inner_join(test::tabPerson)));
static_assert(not sqlpp::is_table(test::tabEmpty.left_outer_join(test::tabPerson)));
static_assert(not sqlpp::is_table(test::tabEmpty.right_outer_join(test::tabPerson)));
static_assert(not sqlpp::is_table(test::tabEmpty.outer_join(test::tabPerson)));

// raw tables
static_assert(sqlpp::is_table(test::tabEmpty));
static_assert(sqlpp::is_table(test::tabPerson));
static_assert(sqlpp::is_table(test::tabDepartment));

// aliased tables
static_assert(sqlpp::is_table(test::tabDepartment.as(foo)));
static_assert(sqlpp::is_table(test::tabDepartment.as(test::tabEmpty)));
static_assert(sqlpp::is_table(test::tabDepartment.as(test::tabPerson.id)));

// conditional or cross joins
static_assert(sqlpp::is_table(test::tabEmpty.cross_join(test::tabPerson)));
static_assert(sqlpp::is_table(test::tabEmpty.join(test::tabPerson).unconditionally()));
static_assert(sqlpp::is_table(test::tabEmpty.inner_join(test::tabPerson).unconditionally()));
static_assert(sqlpp::is_table(test::tabEmpty.left_outer_join(test::tabPerson).unconditionally()));
static_assert(sqlpp::is_table(test::tabEmpty.right_outer_join(test::tabPerson).unconditionally()));
static_assert(sqlpp::is_table(test::tabEmpty.outer_join(test::tabPerson).unconditionally()));

int main()
{
}
