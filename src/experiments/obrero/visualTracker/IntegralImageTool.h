#ifndef IntegralImageTool_INC
#define IntegralImageTool_INC

#include <yarp/YARPImage.h>

class IntegralImageTool
{
public:
  void Offset(YARPImageOf<YarpPixelFloat>& src, 
	      YARPImageOf<YarpPixelFloat>& dest, 
	      int dx, int dy, int clean=0);

  void Offset(YARPImageOf<YarpPixelMono>& src, 
	      YARPImageOf<YarpPixelMono>& dest, 
	      int dx, int dy, int clean=0);

  void Offset(YARPImageOf<YarpPixelBGR>& src, 
	      YARPImageOf<YarpPixelBGR>& dest, 
	      int dx, int dy, int clean=0);
  
  void MaskOffset(YARPImageOf<YarpPixelFloat>& dest, int delta);
  void MaskOffset(YARPImageOf<YarpPixelMono>& dest, int delta);
  void MaskOffset(YARPImageOf<YarpPixelBGR>& dest, int delta);

  void GetMean(YARPImageOf<YarpPixelFloat>& src, 
	       YARPImageOf<YarpPixelFloat>& dest, 
	       int size);

  void GetVariance(YARPImageOf<YarpPixelFloat>& src, 
		   YARPImageOf<YarpPixelFloat>& dest,
		   int size);
};

#endif
