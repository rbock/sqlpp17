#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/name_tag.h>
#include <sqlpp17/core/value.h>

#include <sqlpp17/postgresql/connection.h>
#include <sqlpp17/postgresql/connection_config.h>

SQLPP_CREATE_NAME_TAG(a);

int main()
{
  sqlpp::postgresql::connection_config_t config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlpp::postgresql::connection_t<> db{config};

  auto rows = db(select(sqlpp::value(false).as(a)));
  return 0;
}