//
// YARPColorConverter.cpp
//

#include "YARPColorConverter.h"

void YARPColorConverter::RGB2Grayscale (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelMono>& out)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	iplColorToGray(in.GetIplPointer(), out.GetIplPointer());
}

void YARPColorConverter::RGB2Grayscale (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelMono>& out)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	iplColorToGray(in.GetIplPointer(), out.GetIplPointer());
}

void YARPColorConverter::RGB2HSV (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelHSV>& out)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	iplRGB2HSV(in.GetIplPointer(), out.GetIplPointer());
}

void YARPColorConverter::RGB2HSV (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelHSV>& out)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	iplRGB2HSV(in.GetIplPointer(), out.GetIplPointer());
}

void YARPColorConverter::RGB2Normalized (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelRGBFloat>& out, float threshold)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	assert (out.GetHeight() == in.GetHeight());
	assert (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	float lum;
	float *tmp;

	for(c = 0; c<in.GetWidth(); c++)
	{
		for(r = 0; r < in.GetHeight(); r++)
		{
			tmp = (float *) outTmp;
			lum = inTmp[0] + inTmp[1] + inTmp[2];
			if (lum > threshold)
			{
				tmp[0] = inTmp[0]/lum;
				tmp[1] = inTmp[1]/lum;
				tmp[2] = inTmp[2]/lum;
			}
			else
			{
				tmp[0] = 0.0;
				tmp[1] = 0.0;
				tmp[2] = 0.0;
			}
			
			inTmp += 3;
			outTmp += 3*sizeof(float);
		}
		inTmp += padIn;
		outTmp += padOut;
	}
		
}

void YARPColorConverter::RGB2Normalized (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelRGB>& out, float threshold)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	assert (out.GetHeight() == in.GetHeight());
	assert (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	float lum;
	
	for(c = 0; c<in.GetWidth(); c++)
	{
		for(r = 0; r < in.GetHeight(); r++)
		{
			lum = inTmp[0] + inTmp[1] + inTmp[2];
			if (lum > threshold)
			{
				outTmp[0] = (inTmp[0]/lum)*255 + 0.5;
				outTmp[1] = (inTmp[1]/lum)*255 + 0.5;
				outTmp[2] = (inTmp[2]/lum)*255 + 0.5;
			}
			else
			{
				outTmp[0] = 0.0;
				outTmp[1] = 0.0;
				outTmp[2] = 0.0;
			}
			
			inTmp += 3;
			outTmp += 3;
		}
		inTmp += padIn;
		outTmp += padOut;
	}
		
}

// this function now is exactly like the RGB one; however, in the future we may want to use
// different weights for different colors.
void YARPColorConverter::RGB2Normalized (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out, float threshold)
{
	assert (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	assert (out.GetHeight() == in.GetHeight());
	assert (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	float lum;
	
	for(c = 0; c<in.GetWidth(); c++)
	{
		for(r = 0; r < in.GetHeight(); r++)
		{
			lum = inTmp[0] + inTmp[1] + inTmp[2];
			if (lum > threshold)
			{
				outTmp[0] = (inTmp[0]/lum)*255 + 0.5;	// B
				outTmp[1] = (inTmp[1]/lum)*255 + 0.5;	// G
				outTmp[2] = (inTmp[2]/lum)*255 + 0.5;	// R
			}
			else
			{
				outTmp[0] = 0.0;
				outTmp[1] = 0.0;
				outTmp[2] = 0.0;
			}
			
			inTmp += 3;
			outTmp += 3;
		}
		inTmp += padIn;
		outTmp += padOut;
	}
		
}