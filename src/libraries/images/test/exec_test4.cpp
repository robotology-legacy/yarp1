#include <iostream.h>

#include "YARPImage.h"


void main()
{
  YARPImageOf<YarpPixelRGB> im1;
  YARPImageOf<YarpPixelBGR> im2;
  YARPGenericImage im3;
  
  im1.Resize(128,128);
  cout << "Step 1" << endl;
  im3.PeerCopy(im1);
  cout << "Step 2" << endl;
  im2.Refer(im3);
  cout << "Step 3" << endl;
}

