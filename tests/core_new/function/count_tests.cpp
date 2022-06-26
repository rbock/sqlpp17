#include <sqlpp17/core/aggregate.h>
#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/flags.h>
#include <sqlpp17/core/function/count.h>
#include <sqlpp17/core/function/max.h>
#include <sqlpp17/core/star.h>

#include <assert_bad_expression.h>
#include <tables/tab_person.h>

#include <catch2/catch_test_macros.hpp>

using ::test::tabPerson;

namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_count_arg_is_expression, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_count_arg_is_not_alias, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_count_arg_is_not_aggregate, T...> = true;
}  // namespace sqlpp

SQLPP_CREATE_NAME_TAG(tag);

TEST_CASE("Serialize count statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("count")
  {
    REQUIRE(serialize(count(tabPerson.id)) == "COUNT(tab_person.id)");
  }

  SECTION("count distinct")
  {
    REQUIRE(serialize(count(sqlpp::distinct, tabPerson.id)) == "COUNT(DISTINCT tab_person.id)");
  }
}

TEST_CASE("Construct good count statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("numeric literal")
  {
    sqlpp::test::assert_good_expression(sqlpp::count(5));
  }

  // true: mysql, sqlite3
  // false: postgresql
  SECTION("non-numeric literal")
  {
    sqlpp::test::assert_good_expression(sqlpp::count("test"));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("column")
  {
    sqlpp::test::assert_good_expression(count(tabPerson.name));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("all")
  {
    sqlpp::test::assert_good_expression(count(sqlpp::star));
  }
}

TEST_CASE("Construct bad count statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("subquery (with numeric column)")
  {
    auto subquery = sqlpp::select(tabPerson.id).from(tabPerson).unconditionally();
    sqlpp::test::assert_bad_expression(sqlpp::assert_count_arg_is_expression{}, count(subquery));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with alias arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_count_arg_is_not_alias{}, count(tabPerson.id.as(tag)));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with aggregate arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_count_arg_is_not_aggregate{}, count(max(tabPerson.id)));
  }
}
