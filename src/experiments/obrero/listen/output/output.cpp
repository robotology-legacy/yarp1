#define LINUX_BUILD
#define YARP_OS_CONFIG LINUX

#include <yarp/YARPBottle.h>

int main() {
  YARPInputPortOf<YARPBottle> input;
  input.Register("/in");
  //YARPPort::Connect("/out","/in");
  while (1) {
    if (input.Read()) {
      YARPBottle& bot = input.Content();
      double d1 = 0;
      bot.readFloat(&d1);
      double d2 = 0;
      bot.readFloat(&d2);
      int x = (int)(d2*10+0.5);
      if (x<0) x = 0;
      if (x>70) x = 70;
      //char txt[] = "                                                                               ";
      char txt[] = ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>";
      txt[x] = '\0';
      //printf("%g\n", d2);
      printf("%sX\n", txt);
    }
  }
  return 0;
}
