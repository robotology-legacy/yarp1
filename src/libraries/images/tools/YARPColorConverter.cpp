/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPColorConverter.cpp,v 1.5 2003-12-05 16:16:21 babybot Exp $
///
///

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

	for(r = 0; r<in.GetHeight(); r++)
	{
		for(c = 0; c < in.GetWidth(); c++)
		{
			tmp = (float *) outTmp;
			lum = (float)( inTmp[0] + inTmp[1] + inTmp[2]);
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
