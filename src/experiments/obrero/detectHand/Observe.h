
#include <yarp/YARPImage.h>

class Observe {
public:
  void beginObservation();
  void addObservation(YARPImageOf<YarpPixelRGB>& src,
		      int x, int y, float q);
  void endObservation();
  float Apply(YARPImageOf<YarpPixelRGB>& src, 
	      YARPImageOf<YarpPixelRGB>& dest);
};
