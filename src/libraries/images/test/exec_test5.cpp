#include <iostream.h>
#include <sys/kernel.h>

#include "YARPImage.h"


void main()
{
  YARPImageOf<YarpPixelRGB> im1;
  char buf[3*50*50];
  
  while (1)
    {
      im1.UncountedRefer(buf,50,50);
      delay(10);
    }
}

