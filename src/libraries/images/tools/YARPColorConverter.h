//
// YARPColorConverter.h
//

//
// a few color space conversion methods.

#ifndef __YARPColorConverterh__
#define __YARPColorConverterh__

#include "YARPImage.h"
#include "YARPFilters.h"

//
class YARPColorConverter
{
public:
	static void RGB2HSV (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelHSV>& out);
	static void RGB2HSV (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelHSV>& out);
	static void RGB2Grayscale (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out);
	static void RGB2Grayscale (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out);
	static void RGB2Normalized (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelRGBFloat>& out, float threshold);
	static void RGB2Normalized (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelRGB>& out, float threshold);
	static void RGB2Normalized (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out, float threshold);
};

#endif