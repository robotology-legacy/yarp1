#include "YARPPort.h"

extern int __debug_level;

void main(int argc, char *argv[])
{
//  __debug_level = 100;
  if (argc==3)
    {
      YARPPort::Connect(argv[1],argv[2]);
      //int pid = p.GetServer(argv[1]);
      //p.SayServer(pid,argv[2]);
    }
}
