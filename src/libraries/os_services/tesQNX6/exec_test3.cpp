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
      } while (!id.IsValid());

      int x = 42;
      while (1)
	{
	  out.Wait();
	  cout << "Preparing to send: " << txt << endl;
	  cout.flush();
	  out.Post();

	  YARPMultipartMessage smsg(2);
	  smsg.Set(0,txt,sizeof(txt));
	  smsg.Set(1,(char*)(&x),sizeof(x));
	  YARPMultipartMessage rmsg(2);
	  double y = 999;
	  rmsg.Set(0,reply,sizeof(reply));
	  rmsg.Set(1,(char*)(&y),sizeof(y));
	  cout << "***sending: " << txt << endl;
	  cout.flush();
	  YARPSyncComm::Send(id,smsg,rmsg);
	  x++;

	  out.Wait();
	  cout << "Got reply : " << reply << " and number " << y << endl;
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
	  YARPMultipartMessage imsg(2);
	  int x = 999;
	  imsg.Set(0,buf,sizeof(buf));
	  imsg.Set(1,(char*)(&x),sizeof(x));
	  YARPNameID id = YARPSyncComm::BlockingReceive(YARPNameID(),
							imsg);
	  out.Wait();
	  sprintf(reply,"<%s,%d>", buf, x);
	  cout << "Received message: " << buf << " and number " << x << endl;
	  cout.flush();
	  out.Post();
	  double y = 432.1;
	  YARPMultipartMessage omsg(2);
	  omsg.Set(0,reply,sizeof(reply));
	  omsg.Set(1,(char*)(&y),sizeof(y));
	  YARPSyncComm::Reply(id,omsg);
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

