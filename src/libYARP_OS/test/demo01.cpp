#include <stdio.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

YARPInputPortOf<int> in_port;
YARPOutputPortOf<int> out_port;

class MyThread : public YARPThread {
public:
  virtual void Body() {
    printf("Thread starting, listening for numbers\n");
    for (int i=0; i<5; i++) {
      if (in_port.Read()) {
	printf("Got number %d\n", in_port.Content());
      }
    }
    printf("Thread finished, not listening anymore\n");
  }
};

extern int __debug_level;


int main() {
  //  __debug_level = 100;

  in_port.Register("/test/demo01/in");
  out_port.Register("/test/demo01/out");
  out_port.Connect("/test/demo01/in");
  
  //YARPTime::DelayInSeconds(10);

  MyThread th;
  th.Begin();
  for (int i=0; i<10; i++) {
    printf("Sending number %d\n", i);
    out_port.Content() = i;
    out_port.Write();
    YARPTime::DelayInSeconds(1);
  }
  in_port.Unregister();
  //out_port.Unregister();

  return 0;
}
