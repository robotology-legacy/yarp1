#include <stdio.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>

int main() {
  YARPOutputPortOf<YARPBottle> out_port;
  out_port.Register("/test/demo00");
  out_port.Connect("/test/reader");

  for (int i=0; i<5; i++) {
    char buf[256];
    sprintf(buf,"test number %d", i);
    out_port.Content().writeText(buf);
    out_port.Write(1);
  }
  out_port.FinishSend();

  return 0;
}
