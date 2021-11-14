#include <sqlpp17/core/aggregate.h>
#include <sqlpp17/core/clause/select.h>
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
  constexpr auto wrong<assert_avg_arg_is_numeric_or_text, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_avg_arg_is_not_alias, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_avg_arg_is_not_aggregate, T...> = true;
}  // namespace sqlpp

SQLPP_CREATE_NAME_TAG(tag);

TEST_CASE("Serialize average statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("avg")
  {
    REQUIRE(serialize(avg(tabPerson.id)) == "AVG(tab_person.id)");
  }

  SECTION("avg distinct")
  {
    REQUIRE(serialize(avg(sqlpp::distinct, tabPerson.id)) == "AVG(DISTINCT tab_person.id)");
  }
}

TEST_CASE("Construct good average statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("numeric literal")
  {
    sqlpp::test::assert_good_expression(sqlpp::avg(5));
  }

  // true: mysql, sqlite3
  // false: postgresql
  SECTION("non-numeric literal")
  {
    sqlpp::test::assert_good_expression(sqlpp::avg("Test"));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("numeric column")
  {
    sqlpp::test::assert_good_expression(avg(tabPerson.id));
  }

  // true: mysql, sqlite3
  // false: postgresql
  SECTION("non numeric column")
  {
    sqlpp::test::assert_good_expression(avg(tabPerson.name));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("arithmetic column expression")
  {
    sqlpp::test::assert_good_expression(avg(tabPerson.id + tabPerson.id));
  }
}

TEST_CASE("Construct bad average statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("subquery (with numeric column)")
  {
    auto subquery = sqlpp::select(tabPerson.id).from(tabPerson).unconditionally();
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_numeric_or_text{}, avg(subquery));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with alias arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_not_alias{}, avg(tabPerson.id.as(tag)));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with aggregate arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_avg_arg_is_not_aggregate{}, avg(count(tabPerson.id)));
  }
}
