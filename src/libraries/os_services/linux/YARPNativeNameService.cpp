#include <stdlib.h>

#include "YARPTime.h"
#include "YARPNativeNameService.h"


int YARPNativeNameService::RegisterName(const char *name)
{
  return -1;
}

YARPNameID YARPNativeNameService::LocateName(const char *name)
{
  return YARPNameID();
}


int YARPNativeNameService::IsNonTrivial()
{
  return 0;
}
