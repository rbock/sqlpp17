
#include "sqlpp17/core/to_sql_string.h"
#include <sqlpp17/core/context_base.h>
#include <sqlpp17/core/star.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Serialize asterics statement")
{
  auto serialize = [](const auto& expr) { return sqlpp::to_sql_string_c(sqlpp::context_base{}, expr); };

  SECTION("star")
  {
    REQUIRE(serialize(sqlpp::star) == "*");
  }
}
