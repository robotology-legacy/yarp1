#include <stdlib.h>
#include <sys/name.h>
#include <errno.h>
#include <string.h>

#include "YARPTime.h"
#include "YARPNativeNameService.h"
#include "YARPNameID_defs.h"
#include "YARPSemaphore.h"

YARPSemaphore namer(1);

int YARPNativeNameService::RegisterName(const char *name)
{
  int name_status = -1;
  int retries = 0;
  int merr = 0;
  char buf[256];
  if (name != NULL)
    {
      strncpy(buf,name,sizeof(buf));
      do
	{
	  namer.Wait();
	  name_status = qnx_name_attach(0, buf);
	  merr = errno;
	  YARPTime::DelayInSeconds(0.05);
	  namer.Post();
	  if (name_status==-1)
	    {
	      YARPTime::DelayInSeconds(0.2);
	      fprintf(stderr, "Getting hassle while registering name %s\n", buf, merr);
//	      sprintf(buf,"%s-%d", name, retries);
	    }
	  retries++;
	} while (name_status == -1 && (merr == EBUSY || retries<20));
      if (name_status == -1)
	{
	  fprintf(stderr, "Problem registering name %s [%d, %d]\n",
		  buf, name_status, errno);
	}
      else
	{
	  name_status = 0;
	}
    }
  return name_status;
}

YARPNameID YARPNativeNameService::LocateName(const char *name)
{
  pid_t pid_server = -1;
  int attempt_count = 0;
  
  if (name!=NULL)
    {
      /*
      while ((attempt_count < 10) &&
	     (-1 == (pid_server = qnx_name_locate(0,name, 0, NULL))))
	{
	  YARPTime::DelayInSeconds(0.2);
	  attempt_count++;
	}
      */
      pid_server = qnx_name_locate(0,name, 0, NULL);
    }
  return YARPNameID(YARP_NAME_MODE_NATIVE,pid_server);
}


int YARPNativeNameService::IsNonTrivial()
{
  return 1;
}
