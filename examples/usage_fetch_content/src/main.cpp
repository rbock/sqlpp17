#include <sqlpp17/name_tag.h>
#include <sqlpp17/select.h>

SQLPP_CREATE_NAME_TAG(a)

int main()
{
  select(sqlpp::value(false).as(sqlpp::alias::a));
  return 0;
}