#include <sqlpp17/core/aggregate.h>
#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/function/count.h>
#include <sqlpp17/core/function/sum.h>

#include <assert_bad_expression.h>
#include <tables/tab_person.h>

#include <catch2/catch.hpp>

using ::test::tabPerson;

namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_sum_arg_is_numeric, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_sum_arg_is_not_alias, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_sum_arg_is_not_aggregate, T...> = true;
}  // namespace sqlpp

SQLPP_CREATE_NAME_TAG(tag);

TEST_CASE("Serialize sum statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("sum")
  {
    REQUIRE(serialize(sum(tabPerson.id)) == "SUM(tab_person.id)");
  }
}

TEST_CASE("Construct good sum statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("numeric literal")
  {
    sqlpp::test::assert_good_expression(sqlpp::sum(5));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("numeric column")
  {
    sqlpp::test::assert_good_expression(sum(tabPerson.id));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("arithmetic column expression")
  {
    sqlpp::test::assert_good_expression(sum(tabPerson.id + tabPerson.id));
  }
}

TEST_CASE("Construct bad sum statement")
{
  // true: mysql, postgresql, sqlite3
  SECTION("subquery (with numeric column)")
  {
    auto subquery = sqlpp::select(tabPerson.id).from(tabPerson).unconditionally();
    sqlpp::test::assert_bad_expression(sqlpp::assert_sum_arg_is_numeric{}, sum(subquery));
  }

  // true: postgresql
  // false: mysql, sqlite3
  SECTION("non-numeric literal")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_sum_arg_is_numeric{}, sqlpp::sum("Test"));
  }

  // true: postgresql
  // false: mysql, sqlite3
  SECTION("non numeric column")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_sum_arg_is_numeric{}, sum(tabPerson.name));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with alias arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_sum_arg_is_not_alias{}, sum(tabPerson.id.as(tag)));
  }

  // true: mysql, postgresql, sqlite3
  SECTION("wrong expression with aggregate arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_sum_arg_is_not_aggregate{}, sum(count(tabPerson.id)));
  }
}
