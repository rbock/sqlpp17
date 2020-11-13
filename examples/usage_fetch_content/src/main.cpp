#include <sqlpp17/clause/select.h>
#include <sqlpp17/name_tag.h>
#include <sqlpp17/value.h>

SQLPP_CREATE_NAME_TAG(a);

int main()
{
  auto rows = select(sqlpp::value(false).as(a));
  return 0;
}