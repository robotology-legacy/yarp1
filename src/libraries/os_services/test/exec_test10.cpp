#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "YARPTime.h"
#include "YARPSyncComm.h"
#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPNetworkTypes.h"

#define for if(1) for

NetInt32 foo;

YARPInputPortOf<NetInt32> in;
YARPOutputPortOf<NetInt32> out;

extern int __debug_level;

class Thread1 : public YARPThread
{
protected:
  int id;
public:
  void SetID(int n_id)
    { id = n_id; }
  
  virtual void Body()
    {
      char buf[256];
      sprintf(buf,"/etest/foo%d", id);
      in.Register(buf);
      YARPTime::DelayInSeconds(2);
      while (1)
	  {
	  printf("Waiting for input\n");
	  in.Read();
	  printf("Read %d\n", in.Content());
	}
    }
};

#define MAX_T 30

int main(int argc, char *argv[])
{
  Thread1 *t1[MAX_T];
  
  __debug_level = 100;

  for (int i=0; i<MAX_T; i++)
    {
      t1[i] = new Thread1;
      assert(t1[i]!=NULL);
      t1[i]->SetID(i);
    }
  
  for (int i=0; i<MAX_T; i++)
    {
      t1[i]->Begin();
//      YARPTime::DelayInSeconds(0.05);
    }
  
  YARPTime::DelayInSeconds(10);
  
  for (int i=0; i<MAX_T; i++)
    {
      t1[i]->End();
    }
  
  return 0;
}

