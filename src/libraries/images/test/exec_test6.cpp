#include <iostream.h>

#include "YARPColorSpaceTool.h"

#include "YARPImage.h"
#include "YARPImageFile.h"

int main()
{
  YARPImageOf<YarpPixelRGB> img, img2;
  YARPImageFile::Read("test6.ppm",img);
  img2.ScaledCopy(img,50,50);
  YARPImageFile::Write("test6_out.ppm",img2);
  return 0;
}

