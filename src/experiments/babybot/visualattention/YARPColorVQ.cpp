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
/// $Id:
///
///


#include "YARPColorVQ.h"

#define DBGPF1 if (0)


void YARPColorVQ::Resize(int x, int y, int fovea)
{
	tmp1.Resize(x, y);
	tmp2.Resize(x, y);

	width=x;
	height=y;
}


void YARPColorVQ::Variance(YARPImageOf<YarpPixelMono> &src, YARPImageOf<YarpPixelInt> &dst, int size)
{
	iplConvert((IplImage*) src, (IplImage*) tmp1);
	tmp2=tmp1;
	
	iplSquare((IplImage*) tmp1, (IplImage*) tmp1);
	iplBlur((IplImage*) tmp1, (IplImage*) tmp1, size, size, (size-1)/2, (size-1)/2);

	iplBlur((IplImage*) tmp2, (IplImage*) tmp2, size, size, (size-1)/2, (size-1)/2);
	iplSquare((IplImage*) tmp2, (IplImage*) tmp2);

	iplSubtract((IplImage*) tmp1, (IplImage*) tmp2, (IplImage*) dst);
}


void YARPColorVQ::Compactness(YARPImageOf<YarpPixelMono> &src, int fovea, int val, int eps)
{
	int area=0;
	int perimetro=0;

	int w=src.GetWidth();
	int h=src.GetHeight();

	for (int y=fovea; y<h-1; y++) {
		// first column
		int x=0;
		if (abs(src(x,y)-val)<eps) {
			area++;
			if (abs(src(w-1,y-1)-val)<eps)
				perimetro--;
			if (abs(src(x+1,y+1)-val)<eps)
				perimetro--;
			if (abs(src(w-1,y+1)-val)<eps)
				perimetro--;
			if (abs(src(x+1,y-1)-val)<eps)
				perimetro--;
		}

		for (x=1; x<w-1; x++)
			if (abs(src(x,y)-val)<eps) {
				area++;
				if (abs(src(x-1,y-1)-val)<eps)
					perimetro--;
				if (abs(src(x+1,y+1)-val)<eps)
					perimetro--;
				if (abs(src(x-1,y+1)-val)<eps)
					perimetro--;
				if (abs(src(x+1,y-1)-val)<eps)
					perimetro--;
			}

		// last column
		x=w-1;
		if (abs(src(x,y)-val)<eps) {
			area++;
			if (abs(src(x-1,y-1)-val)<eps)
				perimetro--;
			if (abs(src(0,y+1)-val)<eps)
				perimetro--;
			if (abs(src(x-1,y+1)-val)<eps)
				perimetro--;
			if (abs(src(0,y-1)-val)<eps)
				perimetro--;
		}
	}

	perimetro+=4*area;
}


void YARPColorVQ::DominantQuantization(YARPImageOf<YarpPixelBGR> &src, YARPImageOf<YarpPixelBGR> &dst, unsigned char t)
{
	for (int r=0; r<height; r++)
		for (int c=0; c<width; c++) {
			unsigned int qr,qg,qb;
			
			unsigned char or=src(c,r).r;
			unsigned char og=src(c,r).g;
			unsigned char ob=src(c,r).b;

			if ( 0.25*255<or && or<0.75*255 &&
				 0.25*255<og && og<0.75*255 &&
				 0.25*255<ob && ob<0.75*255) {
				int dr=abs(or-og)+abs(or-ob);
				int dg=abs(or-og)+abs(og-ob);
				int db=abs(og-ob)+abs(ob-or);

				if (dr>dg+t && dr>db+t) {
					if (or>og && or>ob)
						qr=255;
					else if (or<og && or<ob)
						qr=0;
					else
						qr=127;
				} else
					qr=127;
				
				if (dg>dr+t && dg>db+t) {
					if (og>or && og>ob)
						qg=255;
					else if (og<or && og<ob)
						qg=0;
					else
						qg=127;
				} else
					qg=127;

				if (db>dg+t && db>dr+t) {
					if (ob>og && ob>or)
						qb=255;
					else if (ob<og && ob<or)
						qb=0;
					else
						qb=127;
				} else
					qb=127;
			} else {
				if (or>0.75*255) qr=255;
				else if (or<0.25*255) qr=0;
				else qr=127;

				if (og>0.75*255) qg=255;
				else if (og<0.25*255) qg=0;
				else qg=127;

				if (ob>0.75*255) qb=255;
				else if (ob<0.25*255) qb=0;
				else qb=127;
			}
			
			dst(c,r).r=qr;
			dst(c,r).g=qg;
			dst(c,r).b=qb;
		}
}