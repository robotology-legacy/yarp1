#include <stdio.h>

#include <iostream.h>

#include "YARPSemaphore.h"
#include "YARPThread.h"
#include "YARPTime.h"
#include "YARPSyncComm.h"

#define REG_TEST_NAME "localhost|1111"
#define REG_LOCATE_NAME "localhost|1111"
//#define REG_TEST_NAME "/test/exec_test2"
//#define REG_LOCATE_NAME "/test/exec_test2"

YARPSemaphore out(1);

class MyThread1 : public YARPThread
{
public:
  virtual void Body()
    {
      int ct = 0;
      char txt[128] = "Hello there";
      char reply[128] = "Not set";

      YARPNameID id;
      
      do {
	out.Wait();
	cout << "Looking up name" << endl;
	cout.flush();
	out.Post();
	id = YARPNameService::LocateName(REG_LOCATE_NAME);
	if (!id.IsValid())
	  {
	    YARPTime::DelayInSeconds(0.2);
	  }
      } while (!id.IsValid());
      
      while (1)
	{
	  out.Wait();
	  cout << "Preparing to send: " << txt << endl;
	  cout.flush();
	  out.Post();

	  YARPSyncComm::Send(id,txt,sizeof(txt),reply,sizeof(reply));

	  out.Wait();
	  cout << "Got reply : " << reply << endl;
	  cout.flush();
	  out.Post();
	  
	  YARPTime::DelayInSeconds(2.0);

	  ct++;
	  sprintf(txt, "And a-%d", ct);
	}
    }
};

class MyThread2 : public YARPThread
{
public:
  virtual void Body()
    {
      char buf[128] = "Not set";
      char reply[128] = "Not set";
      int ct = 0;
      YARPTime::DelayInSeconds(0.01);
      YARPNameService::RegisterName(REG_TEST_NAME);
      while (1)
	{
	  out.Wait();
	  cout << "Waiting for input" << endl;
	  cout.flush();
	  out.Post();
	  YARPNameID id = YARPSyncComm::BlockingReceive(YARPNameID(),
							buf,sizeof(buf));
	  sprintf(reply,"Got %s", buf);
	  out.Wait();
	  cout << "Received message: " << buf << endl;
	  cout.flush();
	  out.Post();
	  YARPSyncComm::Reply(id,reply,sizeof(reply));
	  //YARPTime::DelayInSeconds(1.0);
	  ct++;
	}
    }
};

int main()
{
  MyThread1 t1;
  MyThread2 t2;
  t2.Begin();
  YARPTime::DelayInSeconds(1.0);
  t1.Begin();
  YARPTime::DelayInSeconds(10.0);
  t2.End();
  t1.End();
  return 0;
}

