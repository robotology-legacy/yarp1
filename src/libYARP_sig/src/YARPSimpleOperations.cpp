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
///                    #pasa, paulfitz#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPSimpleOperations.cpp,v 1.1 2004-07-13 15:34:21 eshuy Exp $
///
///

//
// YARPSimpleOperations.cpp
//

#include <yarp/YARPSimpleOperations.h>

void YARPSimpleOperation::Scale (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out, double scale)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth() && in.GetHeight() == out.GetHeight());
	ACE_ASSERT (scale <= 1 && scale >= 0);

	const int csize = out.GetIplPointer()->imageSize;
	char *tmpo = out.GetIplPointer()->imageData;
	char *tmpi = in.GetIplPointer()->imageData;

	// not sure about the correctness of this.
	for (int i = 0; i < csize; i++, tmpo++, tmpi++)
	{
		*tmpo = char(*tmpi * scale);
	}
}

void YARPSimpleOperation::Flip (const YARPGenericImage& in, YARPGenericImage& out)
{
	ACE_ASSERT (in.GetIplPointer() != NULL && out.GetIplPointer() != NULL);
	ACE_ASSERT (in.GetWidth() == out.GetWidth() && in.GetHeight() == out.GetHeight());

	const int w = out.GetAllocatedLineSize();
	const int h = out.GetHeight();
	
	char *tmpo = out.GetIplPointer()->imageData + out.GetAllocatedDataSize() - w;
	char *tmpi = in.GetIplPointer()->imageData;

	int i;
	for (i = 0; i < h; i++, tmpi += w, tmpo -= w)
	{
		memcpy (tmpo, tmpi, w);
	}
}

///
///
/*
void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelRGB>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelRGB& pixel)
{
	const int width = dest.GetWidth();
	const int height = dest.GetHeight();

	for(int i = 0; i < 2; i++)
	{
		if(xstart +i < width)
		{
			for(int j=0; j<2; j++)
				if(ystart +j < height)
					dest.Pixel(xstart +i, ystart +j) = pixel;
		}
	}

	const int dx = xend - xstart;
	const int dy = yend - ystart;
	int d = 2 * dy - dx;
	const int incrE = 2 * dy;
	const int incrNE = 2 * (dy - dx);

	while(xstart < xend)
    {
		if (d <= 0)
		{
			d += incrE;
			xstart++;
		}
		else
		{
			d += incrNE;
			xstart++;
			ystart++;
		}

		dest.Pixel(xstart, ystart) = pixel;
    } 
}


void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelBGR>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelBGR& pixel)
{
	const int width = dest.GetWidth();
	const int height = dest.GetHeight();

	for(int i = 0; i < 2; i++)
	{
		if(xstart +i < width)
		{
			for(int j=0; j<2; j++)
				if(ystart +j < height)
					dest.Pixel(xstart +i, ystart +j) = pixel;
		}
	}

	const int dx = xend - xstart;
	const int dy = yend - ystart;
	int d = 2 * dy - dx;
	const int incrE = 2 * dy;
	const int incrNE = 2 * (dy - dx);

	while(xstart < xend)
    {
		if (d <= 0)
		{
			d += incrE;
			xstart++;
		}
		else
		{
			d += incrNE;
			xstart++;
			ystart++;
		}

		dest.Pixel(xstart, ystart) = pixel;
    } 
}


void YARPSimpleOperation::DrawLine (YARPImageOf<YarpPixelMono>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelMono& pixel)
{
	const int width = dest.GetWidth();
	const int height = dest.GetHeight();

	for(int i = 0; i < 2; i++)
	{
		if(xstart +i < width)
		{
			for(int j=0; j<2; j++)
				if(ystart +j < height)
					dest.Pixel(xstart +i, ystart +j) = pixel;
		}
	}

	const int dx = xend - xstart;
	const int dy = yend - ystart;
	int d = 2 * dy - dx;
	const int incrE = 2 * dy;
	const int incrNE = 2 * (dy - dx);

	while(xstart < xend)
    {
		if (d <= 0)
		{
			d += incrE;
			xstart++;
		}
		else
		{
			d += incrNE;
			xstart++;
			ystart++;
		}

		dest.Pixel(xstart, ystart) = pixel;
    } 
}

*/
void YARPSimpleOperation::Threshold (const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out, unsigned char threshold)
{
	iplThreshold(in, out, threshold);
}
