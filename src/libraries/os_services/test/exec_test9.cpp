#include <stdio.h>
#include <string.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"

NetInt32 foo;

YARPInputPortOf<NetInt32> in;
YARPOutputPortOf<NetInt32> out;

extern int __debug_level;

class Thread1 : public YARPThread
{
public:
  virtual void Body()
    {
      in.Register("/frog");
      YARPTime::DelayInSeconds(2);
      while (1)
	  {
	  printf("Waiting for input\n");
	  in.Read();
	  printf("Read %d\n", in.Content());
	}
    }
};

class Thread2 : public YARPThread
{
public:
  virtual void Body()
    {
      out.Register("/fly");
      YARPTime::DelayInSeconds(2);
      printf("Step1\n");
      //out.Connect("/frog");
      printf("*** porter /fly /frog\n");
      printf("Step1.5\n");
      YARPTime::DelayInSeconds(2);
      int ct = 1;
      while (1)
	{
	  printf("*** porter /fly /frog\n");
	  printf("Step2\n");
	  out.Content() = ct;
	  printf("Step3\n");
	  ct++;
	  printf("Writing %d\n", out.Content());
	  out.Write();
	  YARPTime::DelayInSeconds(4);
	}
    }
};

int main(int argc, char *argv[])
{
  Thread1 t1;
  Thread2 t2;
  int s = 1, c = 1;
  
  __debug_level = 100;
  
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

