
#define LINUX_BUILD
#define YARP_OS_CONFIG LINUX

#include <yarp/YARPTime.h>
#include <yarp/YARPBottle.h>

class SignalOutput {
private:
  YARPOutputPortOf<YARPBottle> out_port;
public:
  SignalOutput(const char *name) {
    out_port.Register(name);
  }
  void add(double v) {
    double now = YARPTime::GetTimeAsSeconds();
    out_port.Content().writeFloat(now);
    out_port.Content().writeFloat(v);
    out_port.Write();
  }
};

