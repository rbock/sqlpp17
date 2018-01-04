/*
Copyright (c) 2017, Roland Bock
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

#include <iostream>

#include <connections/mock_db.h>
#include <tables/TabDepartment.h>
#include <tables/TabEmpty.h>
#include <tables/TabPerson.h>

#include <sqlpp17/clause/erase.h>
#include <sqlpp17/operator.h>

int main()
{
  auto db = ::sqlpp::test::mock_db{};

  // default way of constructing an erase statement
  (void)db(sqlpp::erase_from(test::tabPerson).where(test::tabPerson.id == 17));

  // pseudo default way of constructing an erase statement
  (void)db(sqlpp::erase().from(test::tabPerson).where(test::tabPerson.id == 17));

  // pseudo default way of constructing an erase statement
  (void)db(sqlpp::erase().from(test::tabPerson).unconditionally());

  // using << concatenation
  (void)db(sqlpp::erase() << sqlpp::from(test::tabPerson) << sqlpp::where(test::tabPerson.isManager));
}
