#include <stdio.h>
#include <string.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"

NetInt32 foo;

#define LEN 60000
class Msg
{
public:
  char msg[LEN];
};

YARPInputPortOf<Msg> in;
YARPOutputPortOf<Msg> out;

class Thread1 : public YARPThread
{
public:
  virtual void Body()
    {
      in.Register("/foo/the/rampaging/frog");
      //YARPTime::DelayInSeconds(2);
      while (1)
	  {
	  printf("Waiting for input\n");
	  in.Read();
	  printf("Read %s\n", in.Content().msg);
	}
    }
};

class Thread2 : public YARPThread
{
public:
  virtual void Body()
    {
      out.Register("/foo/the/rampaging/fly");
      //YARPTime::DelayInSeconds(2);
      printf("Step1\n");
      out.Connect("/foo/the/rampaging/frog");
      printf("Step1.5\n");
      //YARPTime::DelayInSeconds(2);
      int ct = 1;
      while (1)
	{
	  printf("Step2\n");
	  sprintf(out.Content().msg,"foo");
	  printf("Step3\n");
	  ct++;
	  printf("Writing %s\n", out.Content().msg);
	  //	  out.Write(true);
	  out.Write(true);
//	  YARPTime::DelayInSeconds(2);
	}
    }
};

extern int __debug_level;

int main(int argc, char *argv[])
{
  __debug_level = 98;
  Thread1 t1;
  Thread2 t2;
  int s = 1, c = 1;
  if (argc>=2)
    {
      s = c = 0;
      for (unsigned int i=0; i<strlen(argv[1]); i++)
	{
	  if (argv[1][i] == 's') s=1;
	  if (argv[1][i] == 'c') c=1;
	}
    }
  
  if (s)
    {
      t1.Begin();
    }

  if (c)
    {
      if (s)
	{
	  YARPTime::DelayInSeconds(1);
	}
      t2.Begin();
    }

  YARPTime::DelayInSeconds(1000000.0);
  return 0;
}

