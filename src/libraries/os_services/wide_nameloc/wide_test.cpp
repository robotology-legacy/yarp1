#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "YARPSocket.h"
#include "YARPTime.h"
#include "wide_nameloc.h"

#define DEFAULT_MACHINE "localhost"
#define DEFAULT_PORT 4200
#define DEFAULT_NAME "/foo/first"

void run_test(const char *machine, int port, const char *name, 
	      int registering, int local_port)
{
  printf("Starting up test for nameloc service on machine %s, port %d\n", 
	 machine, port);
  printf("Intend to %s the name %s\n", 
	 registering?"register":"look up",
	 name);

  YARPInputSocket is;
  char buf2[256];
  sprintf(buf2,"|%d", local_port);
  is.Register(buf2);
  int in_port = is.GetAssignedPort();
  printf("Assigned port %d\n", in_port);


  char buf[256];
  sprintf(buf,"%s|%d", machine, port);

  YARPOutputSocket os;
  os.Connect(buf);

  NameServiceHeader hdr;
  hdr.port = in_port;
  hdr.machine_length = 0;
  hdr.key_length = strlen(name)+1;
  hdr.spare2 = hdr.spare1 = -1;
  char hostname[256] = "localhost";
  if (registering)
    {
      YARPNetworkObject::GetHostName(hostname,sizeof(hostname));
      hdr.machine_length = strlen(hostname)+1;
      hdr.request_type = NAME_REGISTER;
    }
  else
    {
      hdr.request_type = NAME_LOOKUP;
    }

  os.SendBegin((char*)(&hdr),sizeof(hdr));
  if (registering)
    {
      os.SendContinue(hostname,hdr.machine_length);
    }
  os.SendContinue((char*)name,hdr.key_length);
  char reply[256];
  int result = os.SendEnd(reply,sizeof(reply));
  if (result>=0)
    {
      printf("Got reply: %s\n", reply);
    }
  else
    {
      printf("No reply\n");
    }

  //  YARPTime::DelayInSeconds(5);
  //printf("Finished\n");
}

int main(int argc, char *argv[])
{
  const char *machine = DEFAULT_MACHINE;
  int port = DEFAULT_PORT;
  const char *name = DEFAULT_NAME;
  int local_port = 0;
  int registering = 0;
  if (argc>=3)
    {
      machine = argv[1];
      port = atoi(argv[2]);
      if (port==0) { port = DEFAULT_PORT; }
    }
  if (argc>=4)
    {
      name = argv[3];
    }
  if (argc>=5)
    {
      registering = atoi(argv[4]);
    }
  if (argc>=6)
    {
      local_port = atoi(argv[5]);
    }
  run_test(machine,port,name,registering,local_port);
  return 0;
}

