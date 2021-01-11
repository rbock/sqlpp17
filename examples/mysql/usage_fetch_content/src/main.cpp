#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/name_tag.h>
#include <sqlpp17/core/value.h>

#include <sqlpp17/mysql/connection.h>
#include <sqlpp17/mysql/connection_config.h>

SQLPP_CREATE_NAME_TAG(a);

int main()
{
  sqlpp::mysql::connection_config_t config;
  config.path_to_database = ":memory:";
  config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;

  sqlpp::mysql::connection_t<> db{config};

  auto rows = db(select(sqlpp::value(false).as(a)));
  return 0;
}