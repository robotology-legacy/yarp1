#include <stdlib.h>
//#include <sys/name.h>
#include <sys/dispatch.h> //+QNX6+
#include <errno.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include <stdio.h>
#include <sys/iofunc.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "YARPTime.h"
#include "YARPNativeNameService.h"
#include "YARPNameID_defs.h"
#include "YARPSemaphore.h"
#include "YARPNameManager.h"

/*************************PRINCIPAL******************************************/

int register_name(const char *,int);
int name_search(const char *);

YARPSemaphore namer(1);

int YARPNativeNameService::RegisterName(const char *name)
{
  int name_status = -1;
  name_attach_t *attach;//+QNX6+
  int retries = 0;
  int merr = 0;
  char buf[256];
  int chid;
  
  if (name != NULL)
    {
      strncpy(buf,name,sizeof(buf));
      do
	{
	  namer.Wait();

	  if ((chid = ChannelCreate(NULL)) != -1)
	  {
		 register_name(name,chid);	
		 name_status = chid;	
	  }
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
//	  name_status = 0;
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
     
	  int fd = name_search(name);
	  pid_server = fd;
    }
  return YARPNameID(YARP_NAME_MODE_NATIVE,pid_server);
}


int YARPNativeNameService::IsNonTrivial()
{
  return 1;
}

int register_name(const char * name, int chid)
{
	char hostname[250];
	YARPRegisteredName frname;
	YARPNameManager  * namemanager;
		
	namemanager = new YARPNameManager(FILE_NAME);
	
	assert(namemanager != NULL);
	
	gethostname(hostname,256);
	YARPRegisteredName	rname(name,
						  hostname,
						  getpid(),
						  chid);
	
	namemanager->Add(rname);		
	//namemanager->List();	
	delete namemanager;		
	
}

int name_search(const char * name)
{
	YARPNameManager  * namemanager;
	YARPRegisteredName rname;
	int coid = 0;
	   
	namemanager = new YARPNameManager(FILE_NAME);
  	
  	if(namemanager->FindName((char *)name,rname))
  	{
  		coid = ConnectAttach( netmgr_strtond(rname.GetNode(),NULL),
  					   rname.GetProcId(),
  					   rname.GetChId(),
  					   0,
  					   0);
 		//cout << "I have found: "<< coid << " " << rname ;
  	}      
       
    delete namemanager;   
    return coid;
}


