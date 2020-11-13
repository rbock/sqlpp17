#include <sqlpp17/clause/select.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/value.h>

#include <sqlpp17/sqlite3/connection.h>
#include <sqlpp17/sqlite3/connection_config.h>

SQLPP_CREATE_NAME_TAG(a);

int main()
{
  sqlpp::sqlite3::connection_config_t config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlpp::sqlite3::connection_t<> db{config};

  auto rows = db(select(sqlpp::value(false).as(a)));
  return 0;
}