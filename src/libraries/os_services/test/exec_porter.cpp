#include "YARPPort.h"

extern int __debug_level;

void main(int argc, char *argv[])
{
  if (argc==3)
    {
      YARPPort::Connect(argv[1],argv[2]);
    }
}
