#include "colorconverter.h"
#include <yarp/YARPConfig.h>

void RGB2Grayscale(const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
{
	ACE_ASSERT (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	ACE_ASSERT (out.GetHeight() == in.GetHeight());
	ACE_ASSERT (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	for(r = 0; r<in.GetHeight(); r++)
	{
		for(c = 0; c < in.GetWidth(); c++)
		{
			float tmp = *inTmp++;
			tmp += *inTmp++;
			tmp += *inTmp++;
			*outTmp = (1/3.0) * tmp;
			
			outTmp++;
		}
		inTmp += padIn;
		outTmp += padOut;
	}
}
void RGB2Grayscale(const YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out)
{
	ACE_ASSERT (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	ACE_ASSERT (out.GetHeight() == in.GetHeight());
	ACE_ASSERT (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	for(r = 0; r<in.GetHeight(); r++)
	{
		for(c = 0; c < in.GetWidth(); c++)
		{
			float tmp = *inTmp++;
			tmp += *inTmp++;
			tmp += *inTmp++;
			*outTmp = (1/3.0) * tmp;
			
			outTmp++;
		}
		inTmp += padIn;
		outTmp += padOut;
	}
}

void RGB2Normalized (const YARPImageOf<YarpPixelRGB>& in, YARPImageOf<YarpPixelRGB>& out, float threshold)
{
	ACE_ASSERT (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	ACE_ASSERT (out.GetHeight() == in.GetHeight());
	ACE_ASSERT (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	float lum;
	
	for(r = 0; r<in.GetHeight(); r++)
	{
		for(c = 0; c < in.GetWidth(); c++)
		{
			lum = (float)(inTmp[0] + inTmp[1] + inTmp[2]);
			if (lum > threshold)
			{
				outTmp[0] = (unsigned char)((inTmp[0]/lum)*255 + 0.5);
				outTmp[1] = (unsigned char)((inTmp[1]/lum)*255 + 0.5);
				outTmp[2] = (unsigned char)((inTmp[2]/lum)*255 + 0.5);
			}
			else
			{
				outTmp[0] = 0;
				outTmp[1] = 0;
				outTmp[2] = 0;
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
void RGB2Normalized (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out, float threshold)
{
	ACE_ASSERT (out.GetIplPointer() != NULL && in.GetIplPointer() != NULL);
	ACE_ASSERT (out.GetHeight() == in.GetHeight());
	ACE_ASSERT (out.GetWidth() == in.GetWidth());

	unsigned char *inTmp = (unsigned char *) in.GetAllocatedArray();
	unsigned char *outTmp = (unsigned char *) out.GetAllocatedArray();

	int r = 0;
	int c = 0;
	int padIn = in.GetPadding();
	int padOut = out.GetPadding();
		
	float lum;
	
	for(r = 0; r<in.GetHeight(); r++)
	{
		for(c = 0; c < in.GetWidth(); c++)
		{
			lum = (float) (inTmp[0] + inTmp[1] + inTmp[2]);
			if (lum > threshold)
			{
				outTmp[0] = (unsigned char)((inTmp[0]/lum)*255 + 0.5);	// B
				outTmp[1] = (unsigned char)((inTmp[1]/lum)*255 + 0.5);	// G
				outTmp[2] = (unsigned char)((inTmp[2]/lum)*255 + 0.5);	// R
			}
			else
			{
				outTmp[0] = 0;
				outTmp[1] = 0;
				outTmp[2] = 0;
			}
			
			inTmp += 3;
			outTmp += 3;
		}
		inTmp += padIn;
		outTmp += padOut;
	}
		
}