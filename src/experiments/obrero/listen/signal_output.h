
#define LINUX_BUILD
#define YARP_OS_CONFIG LINUX

#include <yarp/YARPTime.h>
#include <yarp/YARPBottle.h>

class SignalOutput {
private:
  YARPOutputPortOf<YARPBottle> out_port;
  int count;
public:
  SignalOutput(const char *name) {
    out_port.Register(name);
    count = 0;
  }
  void add(double v) {
    //double now = YARPTime::GetTimeAsSeconds();
    out_port.Content().writeInt(0);
    out_port.Content().writeFloat(v);
    //out_port.Content().writeFloat(now);
    out_port.Content().writeInt(count);
    out_port.Write();
    count++;
  }
};

