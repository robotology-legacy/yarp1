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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPHistoSegmentation.cpp,v 1.1 2004-12-27 15:20:49 beltran Exp $
///
///

#include <yarp/YARPHistoSegmentation.h>
#include <yarp/YARPLogpolar.h>

YARPHistoSegmentation::YARPHistoSegmentation(double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char n):
 YARP3DHistogram(max, min, n)
{
	_lumaThreshold = lumaTh;
	_satThreshold = satTh;
}

YARPHistoSegmentation::YARPHistoSegmentation(double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char *n):
YARP3DHistogram(max, min, n)
{
	_lumaThreshold = lumaTh;
	_satThreshold = satTh;
}

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelBGR> &src)
{
	int i;
	int j;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;

	for(i = 0; i < src.GetHeight(); i++)
	{
		b = (unsigned char *) src.GetArray()[i];
		g = b+1;
 		r = b+2;

//		unsigned char rp, gp, bp;
							
		for(j = 0; j < src.GetWidth(); j++)
		{
			// _normalize(*r, *g, *b, &rp, &gp, &bp);
			// YARP3DHistogram::Apply(rp, gp, bp);
			if (_checkThresholds(YarpPixelRGB(*r,*g,*b)))
				YARP3DHistogram::Apply(*r, *g, *b);
			b += 3;
			g += 3;
			r += 3;
		}
	}

}

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelRGB> &src)
{
	int i;
	int j;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
			
	for(i = 0; i < src.GetHeight(); i++)
	{
		r = (unsigned char *) src.GetArray()[i];
		g = r+1;
		b = r+2;

		// unsigned char rp, gp, bp;
		for(j = 0; j < src.GetWidth(); j++)
		{
			// _normalize(*r, *g, *b, &rp, &gp, &bp);
			// YARP3DHistogram::Apply(rp, gp, bp);
			if (_checkThresholds(YarpPixelRGB(*r,*g,*b)))
				YARP3DHistogram::Apply(*r, *g, *b);
				
			b += 3;
			g += 3;
			r += 3;
		}
	}
}

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelHSV> &src)
{
	int i;
	int j;
	unsigned char *h;
	unsigned char *s;
	unsigned char *v;
			
	for(i = 0; i < src.GetHeight(); i++)
	{
		h = (unsigned char *) src.GetArray()[i];
		s = h+1;
		v = h+2;

		for(j = 0; j < src.GetWidth(); j++)
		{
			if (_checkThresholds(*h,*s,*v))
				YARP3DHistogram::Apply(*h, *s, 0);
			
			h += 3;
			s += 3;
			v += 3;
		}
	}
}

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelRGB *src;
	YarpPixelRGB tmp;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelRGB *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			// _normalize(*src, tmp);
			if (_checkThresholds(*src))
				*dst = (unsigned char)(YARP3DHistogram::backProjection(*src)*255 + 0.5);
			else
				*dst = 0;
			src++;
			dst++;
		}
	}
}

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelBGR *src;
	YarpPixelRGB tmp;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelBGR *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
//			_normalize(*src, tmp);
			if (_checkThresholds(YarpPixelRGB(src->r, src->g, src->b)))
				*dst = (unsigned char)(YARP3DHistogram::backProjection(*src)*255 + 0.5);
			else
				*dst = 0;
			src++;
			dst++;
		}
	}
}

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelHSV> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelHSV *src;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelHSV *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];

		for(i = 0; i < in.GetWidth(); i++)
		{
			if (_checkThresholds(src->h, src->s, src->v))
				*dst = (unsigned char)(YARP3DHistogram::backProjection(src->h, src->s, 0)*255 + 0.5);
			else
				*dst = 0;
			
			src++;
			dst++;
		}
	}
}

double YARPHistoSegmentation::backProjection(const YarpPixelRGB &pixel)
{
	return YARP3DHistogram::backProjection(pixel);
}

double YARPHistoSegmentation::backProjection(const YarpPixelHSV &pixel)
{
	return YARP3DHistogram::backProjection(pixel);
}

//
// logpolar class differs only in the apply methods
void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelBGR> &src)
{
	int i;
	int j;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;

	for(i = 0; i < src.GetHeight(); i++)
	{
		b = (unsigned char *) src.GetArray()[i];
		g = b+1;
 		r = b+2;

//		unsigned char rp, gp, bp;
		double w = pSize(1, i);
		for(j = 0; j < src.GetWidth(); j++)
		{
			// YARPHistoSegmentation::_normalize(*r, *g, *b, &rp, &gp, &bp);
			// YARP3DHistogram::Apply(rp, gp, bp, w);
			if (_checkThresholds(YarpPixelRGB(*r,*g,*b)))
				YARP3DHistogram::Apply(*r, *g, *b);

			b += 3;
			g += 3;
			r += 3;
		}
	}

}

void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelRGB> &src)
{
	int i;
	int j;
	unsigned char *r;
	unsigned char *g;
	unsigned char *b;
			
	for(i = 0; i < src.GetHeight(); i++)
	{
		r = (unsigned char *) src.GetArray()[i];
		g = r+1;
		b = r+2;

//		unsigned char rp, gp, bp;
		double w = pSize(1, i);	
		for(j = 0; j < src.GetWidth(); j++)
		{
			// YARPHistoSegmentation::_normalize(*r, *g, *b, &rp, &gp, &bp);
			// YARP3DHistogram::Apply(rp, gp, bp, w);
			if (_checkThresholds(YarpPixelRGB(*r,*g,*b)))
				YARP3DHistogram::Apply(*r, *g, *b);
				
			b += 3;
			g += 3;
			r += 3;
		}
	}
}

void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelHSV> &src)
{
	int i;
	int j;
	unsigned char *h;
	unsigned char *s;
	unsigned char *v;
			
	for(i = 0; i < src.GetHeight(); i++)
	{
		h = (unsigned char *) src.GetArray()[i];
		s = h+1;
		v = h+2;

		double w = pSize(1, i);
		for(j = 0; j < src.GetWidth(); j++)
		{
			if (_checkThresholds(*h,*s,*v))
				YARP3DHistogram::Apply(*h, *s, 0, w);
						
			h += 3;
			s += 3;
			v += 3;
		}
	}
}
