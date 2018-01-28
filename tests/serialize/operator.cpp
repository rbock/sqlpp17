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

#include <sqlpp17/operator.h>
#include <sqlpp17/value.h>

#include <sqlpp17_test/tables/TabDepartment.h>
#include <sqlpp17_test/tables/TabEmpty.h>
#include <sqlpp17_test/tables/TabPerson.h>

#include "assert_equality.h"

using ::sqlpp::test::assert_equality;
using ::test::tabPerson;

int main()
{
  try
  {
    // Logical
    assert_equality("tab_person.is_manager AND 1", tabPerson.isManager and true);
    assert_equality("tab_person.is_manager AND 1 AND 0", tabPerson.isManager and true and false);
    assert_equality("tab_person.is_manager AND 0", tabPerson.isManager and (true and false));
    assert_equality("1 AND (tab_person.is_manager AND 0)", true and (tabPerson.isManager and false));

    assert_equality("tab_person.is_manager OR 1", tabPerson.isManager or true);
    assert_equality("tab_person.is_manager OR 1 OR 0", tabPerson.isManager or true or false);
    assert_equality("tab_person.is_manager OR 1", tabPerson.isManager or (true or false));
    assert_equality("1 OR (tab_person.is_manager OR 0)", true or (tabPerson.isManager or false));

    assert_equality("(tab_person.is_manager AND 1) OR 1", tabPerson.isManager and true or true);
    assert_equality("tab_person.is_manager OR 0", tabPerson.isManager or (true and false));
    assert_equality("1 OR (tab_person.is_manager AND 0)", true or tabPerson.isManager and false);
    assert_equality("1 AND (tab_person.is_manager OR 0)", true and (tabPerson.isManager or false));

    assert_equality("NOT tab_person.is_manager", not tabPerson.isManager);
    assert_equality("NOT (NOT tab_person.is_manager)", not not tabPerson.isManager);

    // Comparison
    assert_equality("tab_person.name < 'Herb'", tabPerson.name < "Herb");
    assert_equality("tab_person.name <= 'Herb'", tabPerson.name <= "Herb");
    assert_equality("tab_person.name = 'Herb'", tabPerson.name == "Herb");
    assert_equality("tab_person.name >= 'Herb'", tabPerson.name >= "Herb");
    assert_equality("tab_person.name > 'Herb'", tabPerson.name > "Herb");
    assert_equality("tab_person.name != 'Herb'", tabPerson.name != "Herb");
    assert_equality("tab_person.name LIKE 'Herb%'", tabPerson.name.like("Herb%"));

    assert_equality("'Herb' < tab_person.name", "Herb" < tabPerson.name);
    assert_equality("'Herb' <= tab_person.name", "Herb" <= tabPerson.name);
    assert_equality("'Herb' = tab_person.name", "Herb" == tabPerson.name);
    assert_equality("'Herb' >= tab_person.name", "Herb" >= tabPerson.name);
    assert_equality("'Herb' > tab_person.name", "Herb" > tabPerson.name);
    assert_equality("'Herb' != tab_person.name", "Herb" != tabPerson.name);
    assert_equality("'Herb' LIKE tab_person.name", like("Herb", tabPerson.name));

    // Arithmetic
    assert_equality("tab_person.id / 17", tabPerson.id / 17);
    assert_equality("tab_person.id - 17", tabPerson.id - 17);
    assert_equality("tab_person.id % 17", tabPerson.id % 17);
    assert_equality("tab_person.id * 17", tabPerson.id * 17);
    assert_equality("-tab_person.id", -tabPerson.id);
    assert_equality("tab_person.id + 17", tabPerson.id + 17);

    assert_equality("tab_person.id / 17 / 4", (tabPerson.id / 17) / 4);
    assert_equality("(tab_person.id - 17) / 4", (tabPerson.id - 17) / 4);
    assert_equality("(tab_person.id % 17) / 4", (tabPerson.id % 17) / 4);
    assert_equality("(tab_person.id * 17) / 4", (tabPerson.id * 17) / 4);
    assert_equality("(-tab_person.id) / 4", -tabPerson.id / 4);
    assert_equality("(tab_person.id + 17) / 4", (tabPerson.id + 17) / 4);

    assert_equality("4 + (tab_person.id / 17)", 4 + tabPerson.id / 17);
    assert_equality("(4 + tab_person.id) - 17", 4 + tabPerson.id - 17);
    assert_equality("4 + (tab_person.id % 17)", 4 + tabPerson.id % 17);
    assert_equality("4 + (tab_person.id * 17)", 4 + tabPerson.id * 17);
    assert_equality("4 + (-tab_person.id)", 4 + -tabPerson.id);
    assert_equality("4 + tab_person.id + 17", 4 + tabPerson.id + 17);

    assert_equality("-(-tab_person.id)", -(-tabPerson.id));

    // Binary
    assert_equality("tab_person.id & 17", tabPerson.id & std::int64_t{17});
    assert_equality("tab_person.id | 17", tabPerson.id | std::int64_t{17});
    assert_equality("tab_person.id ^ 17", tabPerson.id ^ std::int64_t{17});
    assert_equality("~tab_person.id", ~tabPerson.id);
    assert_equality("tab_person.id << 2", tabPerson.id << 2);
    assert_equality("tab_person.id >> 2", tabPerson.id >> 2);
  }
  catch (const std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
