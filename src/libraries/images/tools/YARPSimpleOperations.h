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
/// $Id: YARPSimpleOperations.h,v 1.5 2004-02-23 19:50:10 babybot Exp $
///
///

//
// YARPSimpleOperations.h
//

#ifndef __YARPSimpleOperationsh__
#define __YARPSimpleOperationsh__

#include <conf/YARPConfig.h>
#include <ace/config.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

//
// add here simple image manipulation stuff. Scaling, sum, diff.
// It's not by any mean exhaustive...

#include "YARPImage.h"
#include "YARPFilters.h"

const double __crossLength = 2.0;
const double __crossThickness = 1.0;

class YARPSimpleOperation
{
public:
	static void Scale (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out, double scale);
	// decimate or shrinks an image
	// scaleX/scaleY is the actor by which the input image is shrunken (>= 1)
	static void Decimate (const YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelRGB> &out, double scaleX, double scaleY, int interpolate = IPL_INTER_NN);
	static void Decimate (const YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelBGR> &out, double scaleX, double scaleY, int interpolate = IPL_INTER_NN);
	static void Decimate (const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out, double scaleX, double scaleY, int interpolate = IPL_INTER_NN);

	static void Threshold (const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out, unsigned char threshold);
	
	static void Flip (const YARPGenericImage& in, YARPGenericImage& out);

	inline static void Fill (YARPGenericImage &img, int value)
	{
		// image type is not checked!
		char *pointer = img.GetRawBuffer();
		ACE_ASSERT (pointer != NULL);
		
		memset (pointer, value, img.GetAllocatedDataSize());
	}

	static void DrawLine (YARPImageOf<YarpPixelMono>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelMono& pixel);
	static void DrawLine (YARPImageOf<YarpPixelBGR>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelBGR& pixel);
	static void DrawLine (YARPImageOf<YarpPixelRGB>& dest, int xstart, int ystart, int xend, int yend, const YarpPixelRGB& pixel);

	inline static void DrawCross(YARPImageOf<YarpPixelRGB> &img, double dx, double dy, const YarpPixelRGB &pixel, int length = 2, int thick = 1)
	{
		// coordinate center is top-left
		int x = (int) (dx + 0.5);
		int y = (int) (dy + 0.5);

		int i,j,t;
		
		for(t = -thick; t <= thick; t++)
		{
			for(i = -length; i <= length; i++)
				img.SafePixel(x+i,y+t) = pixel;
			for (j = -length; j <= length; j++)
				img.SafePixel(x+t,y+j) = pixel;
		}
	}

	inline static void DrawCross(YARPImageOf<YarpPixelMono> &img, double dx, double dy, unsigned char pixel, int length = 2, int thick = 1)
	{
		// coordinate center is top-left
		int x = (int) (dx + 0.5);
		int y = (int) (dy + 0.5);

		int i,j,t;
		
		for(t = -thick; t <= thick; t++)
		{
			for(i = -length; i <= length; i++)
				img.SafePixel(x+i,y+t) = pixel;
			for (j = -length; j <= length; j++)
				img.SafePixel(x+t,y+j) = pixel;
		}

	}

	inline static void DrawCross(YARPImageOf<YarpPixelBGR> &img, double dx, double dy, const YarpPixelBGR &pixel, int length = 2, int thick = 1)
	{
		// coordinate center is top-left
		int x = (int) (dx + 0.5);
		int y = (int) (dy + 0.5);

		int i,j,t;
		
		for(t = -thick; t <= thick; t++)
		{
			for(i = -length; i <= length; i++)
				img.SafePixel(x+i,y+t) = pixel;
			for (j = -length; j <= length; j++)
				img.SafePixel(x+t,y+j) = pixel;
		}
	}

	inline static int ComputePadding (int linesize, int align)
	{
		int rem = linesize % align;
		return (rem != 0) ? (align - rem) : rem;
	}
};

#endif
