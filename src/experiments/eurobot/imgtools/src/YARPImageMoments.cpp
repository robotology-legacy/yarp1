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
/// $Id: YARPImageMoments.cpp,v 1.1 2004-12-27 15:20:49 beltran Exp $
///
///

#include <yarp/YARPImageMoments.h>
#include <yarp/YARPLogpolar.h>

using namespace _logpolarParams;

// LOGPOLAR VERSION
void YARPLpImageMoments::centerOfMassAndMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y, double *mass)
{
	int t,c;
	double areaT = 0.0;
	double sumX = 0.0;
	double sumY = 0.0;
	unsigned char *src;
	for(c = 0; c < _srho; c++)
	{
		int sumTmpX = 0;
		int sumTmpY = 0;
		int areaTmp = 0;

		src = (unsigned char *)in.GetArray()[c];
		for(t = 0; t < _stheta; t++)
		{
			int tmpX;
			int tmpY;

			Logpolar2Cartesian(c, t, tmpX, tmpY);

			sumTmpX += tmpX*(*src);
			sumTmpY += tmpY*(*src);
			areaTmp += (*src);
					
			src++;
		}
		double J=Jacobian(c, 0);
		sumX+=sumTmpX*J;
		sumY+=sumTmpY*J;
		areaT+=areaTmp*J;


		/*for(t = 0; t < _stheta; t++)
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
		}*/
	}
	
	*mass=areaT;
	
	if (areaT != 0)
	{
		*x = (int)(sumX/areaT + .5);
		*y = (int)(sumY/areaT + .5);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

void YARPLpImageMoments::centralMomentsOrder2(YARPImageOf<YarpPixelMono> &in, int xm, int ym, double *u11, double *u20, double *u02)
{
	int t,c;
	double area = 0.0;
	double res11 = 0.0;
	double res20 = 0.0;
	double res02 = 0.0;
	unsigned char *src;
	
	for(c = 0; c < _srho; c++) {
		int res11Tmp = 0;
		int res20Tmp = 0;
		int res02Tmp = 0;
		
		src = (unsigned char *)in.GetArray()[c];

		for(t = 0; t < _stheta; t++) {
			int x;
			int y;
			
			Logpolar2Cartesian(c, t, x, y);

			res11Tmp += (x-xm)*(y-ym)*(*src);
			res20Tmp += (x-xm)*(x-xm)*(*src);
			res02Tmp += (y-ym)*(y-ym)*(*src);
			
			src++;
		}
		double J = Jacobian(c, 0);
		res11+=res11Tmp*J;
		res20+=res20Tmp*J;
		res02+=res02Tmp*J;
	}

	*u11=res11;
	*u20=res20;
	*u02=res02;
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
void YARPImageMoments::centerOfMassAndMass(YARPImageOf<YarpPixelMono> &in, int *x, int *y, int *mass)
{
	int i,j;
	int area = 0;
	int sumX = 0;
	int sumY = 0;
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
		*x = (int)(sumX/area + .5);
		*y = (int)(sumY/area + .5);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
	*mass = area;
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
