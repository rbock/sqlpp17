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

#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>
#include <sqlpp17/alias_provider.h>

SQLPP_ALIAS_PROVIDER(foo);

// non-tables
static_assert(sqlpp::provided_tables_of(7).empty());
static_assert(provided_tables_of(test::tabPerson.id).empty());
static_assert(provided_tables_of(test::tabPerson.id.as(test::tabPerson)).empty());

// unconditional joins
static_assert(provided_tables_of(test::tabEmpty.join(test::tabPerson)) == type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.inner_join(test::tabPerson)) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.left_outer_join(test::tabPerson)) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.right_outer_join(test::tabPerson)) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.outer_join(test::tabPerson)) ==
              type_set(test::tabEmpty, test::tabPerson));

// raw tables
static_assert(provided_tables_of(test::tabEmpty) == type_set(test::tabEmpty));
static_assert(provided_tables_of(test::tabPerson) == type_set(test::tabPerson));
static_assert(provided_tables_of(test::tabDepartment) == type_set(test::tabDepartment));

// aliased tables
static_assert(provided_tables_of(test::tabDepartment.as(foo)) == type_set(test::tabDepartment.as(foo)));
static_assert(provided_tables_of(test::tabDepartment.as(test::tabEmpty)) ==
              type_set(test::tabDepartment.as(test::tabEmpty)));
static_assert(provided_tables_of(test::tabDepartment.as(test::tabPerson.id)) ==
              type_set(test::tabDepartment.as(test::tabPerson.id)));

// conditional or cross joins
static_assert(provided_tables_of(test::tabEmpty.cross_join(test::tabPerson)) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.join(test::tabPerson).unconditionally()) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.inner_join(test::tabPerson).unconditionally()) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.left_outer_join(test::tabPerson).unconditionally()) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.right_outer_join(test::tabPerson).unconditionally()) ==
              type_set(test::tabEmpty, test::tabPerson));
static_assert(provided_tables_of(test::tabEmpty.outer_join(test::tabPerson).unconditionally()) ==
              type_set(test::tabEmpty, test::tabPerson));

int main()
{
}
