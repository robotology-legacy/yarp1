//
// YARP.h
//	- 
//	- 
//	- 
//

#ifndef __YARPCOLORVQ__
#define __YARPCOLORVQ__

#include <yarp/YARPImage.h>

//
// bounding box type.
class YARPColorVQ {
	YARPImageOf<YarpPixelInt> tmp1;
	YARPImageOf<YarpPixelInt> tmp2;
	
	int height, width;

public:
	
	YARPColorVQ() {}
	YARPColorVQ(int x, int y, int fovea) { Resize(x, y, fovea); }
	~YARPColorVQ() {}

	void Resize(int x, int y, int fovea);
	void Variance(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &dst, int size);
	void Compactness(YARPImageOf<YarpPixelMono> &src, int fovea, int val, int eps);
	void DominantQuantization(YARPImageOf<YarpPixelBGR> &src, YARPImageOf<YarpPixelBGR> &dst, unsigned char t);
	void DominantQuantization(YarpPixelBGR src, YarpPixelBGR &dst, unsigned char t);
};



#endif