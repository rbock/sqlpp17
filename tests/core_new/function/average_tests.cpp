#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/function/avg.h>

#include <core_test/tables/TabPerson.h>

#include <catch2/catch.hpp>

using ::test::tabPerson;

SCENARIO("Constructing avg sql statement")
{
  WHEN("average statement is converted to string")
  {
    std::string avgString = to_sql_string_c(::sqlpp::context_base{}, avg(tabPerson.id));

    THEN("it should match the sql statement")
    {
      REQUIRE(avgString == "AVG(tab_person.id)");
    }
  }

  WHEN("distinct average statement is converted to string")
  {
    std::string avgString = to_sql_string_c(::sqlpp::context_base{}, avg(::sqlpp::distinct, tabPerson.id));

    THEN("it should match the sql statement")
    {
      REQUIRE(avgString == "AVG(DISTINCT tab_person.id)");
    }
  }
}
