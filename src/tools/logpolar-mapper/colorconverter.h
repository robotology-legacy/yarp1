#ifndef __colorconverter__

#include <yarp/YARPImage.h>

void RGB2Normalized(const YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelRGB> &out, float threshold);
void RGB2Normalized(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelBGR> &out, float threshold);
void RGB2Grayscale(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out);
void RGB2Grayscale(const YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out);


#endif

