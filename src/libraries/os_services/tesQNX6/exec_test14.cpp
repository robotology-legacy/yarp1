#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPTime.h"

YARPInputPortOf<int> in_port(YARPInputPort::NO_BUFFERS);
YARPOutputPortOf<int> out_port;

class T1 : public YARPThread
{
public:
  virtual void Body()
    {
      int ct = 0;
      while (1)
	{
	  out_port.Content() = ct;
	  printf("Writing... %d\n", out_port.Content());
	  out_port.Write();
	  out_port.Content() = -1;
	  printf("Writing... %d\n", out_port.Content());
	  out_port.Write(0);
	  ct++;
	}
    }
};

class T2 : public YARPThread
{
public:
  virtual void Body()
    {
      while (1)
	{
	  printf("Reading...\n");
	  in_port.Read();
	  printf("Count is %d\n", in_port.Content());
	  //YARPTime::DelayInSeconds(1);
	  //in_port.Read();
	  //printf("Count is %d\n", in_port.Content());
	}
    }
};


void main()
{
  T1 t1;
  T2 t2;

  in_port.Register("/in");
  out_port.Register("/out");

  YARPTime::DelayInSeconds(0.5);
  out_port.Connect("/in");

  YARPTime::DelayInSeconds(1.0);

  t1.Begin();

  t2.Body();
}

