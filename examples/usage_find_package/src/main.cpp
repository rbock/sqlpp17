#include <sqlpp17/core/clause/select.h>
#include <sqlpp17/core/name_tag.h>
#include <sqlpp17/core/value.h>

SQLPP_CREATE_NAME_TAG(a);

int main()
{
  auto rows = select(sqlpp::value(false).as(a));
  return 0;
}