
#ifndef OBREROVISION_INC
#define OBREROVISION_INC

#include <yarp/YARPImage.h>

class ObreroVision {
private:
  int ox, oy;
  float ov;
public:

  ObreroVision() { ox=oy=0;  ov=0; }

  int getX() { return ox; }

  int getY() { return oy; }

  float getV() { return ov; }

  void Apply(YARPImageOf<YarpPixelRGB>& src,
	     YARPImageOf<YarpPixelRGB>& dest);
};

#endif
