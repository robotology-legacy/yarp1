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
/// $Id: YARPImageMoments.cpp,v 1.8 2004-04-21 17:53:15 natta Exp $
///
///

#include "YARPImageMoments.h"
#include "YARPLogpolar.h"

using namespace _logpolarParams;

// LOGPOLAR VERSION
void YARPLpImageMoments::centerOfMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y)
{
	int t,c;
	double area = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	unsigned char *src;
	for(c = 0; c < _srho; c++)
	{
		src = (unsigned char *)in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			int tmpX = 0;
			int tmpY = 0;
			double J = 0.0;
		
			Logpolar2Cartesian(c, t, tmpX, tmpY);

			J = (*src)*Jacobian(c, t);

			sumX += tmpX*J;
			sumY += tmpY*J;
			area += J;
					
			src++;
		}
	}
	
	if (area != 0)
	{
		*x = (int)(sumX/area);
		*y = (int)(sumY/area);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

double YARPLpImageMoments::centralMoments(YARPImageOf<YarpPixelMono> &in, int xm, int ym, int p, int q)
{
	int t,c;
	double area = 0.0;
	double res = 0.0;
	unsigned char *src;
	for(c = 0; c < _srho; c++)
	{
		src = (unsigned char *)in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			int x = 0;
			int y = 0;
			double J = 0.0;
		
			Logpolar2Cartesian(c, t, x, y);
			J = (*src)*Jacobian(c, t);

			res += pow((double)(x-xm),(double)p)*pow((double)(y-ym),(double)q)*J;
			area += J;
			
			src++;
		}
	}
	
	return res;
}

// CARTESIAN VERSION
void YARPImageMoments::centerOfMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y)
{
	int i,j;
	double area = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	unsigned char *src;
	for(j = 0; j < in.GetHeight() ; j++)
	{
		src = (unsigned char *)in.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			sumX += i*(*src);
			sumY += j*(*src);
			area += *src;
			
			src++;
		}
	}
	
	if (area != 0)
	{
		*x = (int)(sumX/area);
		*y = (int)(sumY/area);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

double YARPImageMoments::centralMoments(YARPImageOf<YarpPixelMono> &in, int xm, int ym, int p, int q)
{
	int i,j;
	double area = 0.0;
	double res = 0.0;
	unsigned char *src;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (unsigned char *)in.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			res += (*src)*pow((double)(i-xm),(double)p)*pow((double)(j-ym),(double)q);
			
			src++;
		}
	}
	
	return res;
}
