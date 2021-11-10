#include <sqlpp17/core/aggregate.h>
#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/function/count.h>
#include <sqlpp17/core/function/max.h>

#include <assert_bad_expression.h>
#include <tables/tab_person.h>

#include <catch2/catch.hpp>

using ::test::tabPerson;

namespace sqlpp
{
  template <typename... T>
  constexpr auto wrong<assert_max_arg_is_expression, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_max_arg_is_not_alias, T...> = true;

  template <typename... T>
  constexpr auto wrong<assert_max_arg_is_not_aggregate, T...> = true;
}  // namespace sqlpp

SQLPP_CREATE_NAME_TAG(tag);

TEST_CASE("Serialize max statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("max")
  {
    REQUIRE(serialize(max(tabPerson.id)) == "MAX(tab_person.id)");
  }
}

TEST_CASE("Construct max statement")
{
  SECTION("good expression with numeric column arg")
  {
    sqlpp::test::assert_good_expression(max(tabPerson.id));
  }
  SECTION("good expression with character column arg")
  {
    sqlpp::test::assert_good_expression(max(tabPerson.name));
  }
#warning Constant should be a good expression?
  // SECTION("good expression with constant")
  // {
  //   sqlpp::test::assert_good_expression(sqlpp::max(5));
  // }
  SECTION("wrong expression with subquery")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_max_arg_is_expression{},
                                       max(sqlpp::select(tabPerson.id).from(tabPerson).unconditionally()));
  }
  SECTION("wrong expression with aggregate arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_max_arg_is_not_aggregate{}, max(count(tabPerson.name)));
  }
  SECTION("wrong expression with alias arg")
  {
    sqlpp::test::assert_bad_expression(sqlpp::assert_max_arg_is_not_alias{}, max(tabPerson.id.as(tag)));
  }
}
