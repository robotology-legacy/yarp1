#include "YARPNativeNameService.h"
#include "YARPPort.h"

extern int __debug_level;

void main(int argc, char *argv[])
{
  if (argc==3)
    {
      YARPNameID r1 = YARPNativeNameService::LocateName(argv[1]);
      if (r1.IsValid())
	{
	  if (argv[2][0] == '/')
	    {
	      YARPNameID r2 = YARPNativeNameService::LocateName(argv[2]);
	      if (r2.IsValid())
		{
		  YARPPort::Connect(argv[1],argv[2]);
		}
	      else
		{
		  printf("Skipping <%s>\n", argv[2]);
		}
	    }
	  else
	    {
	      YARPPort::Connect(argv[1],argv[2]);
	    }
	}
      else
	{
	  printf("Skipping <%s>\n", argv[1]);
	}
    }
}
