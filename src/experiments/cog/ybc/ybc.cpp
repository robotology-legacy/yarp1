#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include "YARPBottle.h"

void main(int argc, char *argv[])
{
  argc--;
  argv++;
  int need_space = 0;
  while (argc>0)
    {
      int x = atoi(argv[0]);
      if (need_space) printf(" ");
      printf("%s", ybc_label(x));
      need_space = 1;
      argc--;
      argv++;
    }
  printf("\n");
}
