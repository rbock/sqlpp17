#include <sqlpp17/core/aggregate.h>
#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/function/avg.h>
#include <sqlpp17/core/function/count.h>

#include <assert_bad_expression.h>
#include <tables/tab_person.h>

#include <catch2/catch.hpp>

using ::test::tabPerson;

namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_avg_arg_is_numeric, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_avg_arg_is_not_alias, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_avg_arg_is_not_aggregate, T...> = true;
}  // namespace sqlpp

SQLPP_CREATE_NAME_TAG(tag);

TEST_CASE("Serialize average statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("AVG")
  {
    REQUIRE(serialize(avg(tabPerson.id)) == "AVG(tab_person.id)");
  }

  SECTION("AVG DISTINCT")
  {
    REQUIRE(serialize(avg(sqlpp::distinct, tabPerson.id)) == "AVG(DISTINCT tab_person.id)");
  }
}

TEST_CASE("Construct average statement")
{
  SECTION("good expression")
  {
    sqlpp::test::assert_good_expression(avg(tabPerson.id));
    sqlpp::test::assert_good_expression(avg(tabPerson.id + tabPerson.id));
  }
  SECTION("wrong expression with non-numeric arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_numeric{}, avg(tabPerson.name));
  }
  SECTION("wrong expression with alias arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_not_alias{}, avg(tabPerson.id.as(tag)));
  }
  SECTION("wrong expression with aggregate arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_not_aggregate{}, avg(count(tabPerson.id)));
  }
}
