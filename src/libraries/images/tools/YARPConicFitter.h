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
/// $Id: YARPConicFitter.h,v 1.6 2003-10-14 12:09:33 babybot Exp $
///
/// Fit simple conics to a segmented region. Logpolar version.
/// September 2003 -- by nat

#ifndef __YARPCONICFITTER__
#define __YARPCONICFITTER__

#include "YARPLogpolar.h"
#include "YARPImageMoments.h"

class YARPShapeCircle
{
public:
	YARPShapeCircle()
	{ theta = 0; rho = 0; radius = 0;};
	~YARPShapeCircle(){};

	int theta;
	int rho;
	int radius;
};

class YARPShapeEllipse
{
public:
	YARPShapeEllipse()
	{ theta = 0; rho = 0; a11 = 0.0; a12 = 0.0; a22 = 0.0; };
	~YARPShapeEllipse(){};

	int theta;
	int rho;
	double a11;
	double a22;
	double a12;
};

class YARPShapeRegion
{
public:
	YARPShapeRegion()
	{
		r = new int [_logpolarParams::_srho*_logpolarParams::_stheta];
		t = new int [_logpolarParams::_srho*_logpolarParams::_stheta];
	}
	~YARPShapeRegion()
	{
		delete [] r;
		delete [] t;
	}

	void reset()
	{ n = 0; }

	void add(int T, int R)
	{ 
		r[n] = R;
		t[n] = T;
		n++;
	}

	int *r;
	int *t;
	int n;
};

class YARPLpConicFitter
{
public:
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// Rmin, Rmax, Rav are respectively min, max and av radius of the region.
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *Rmin, int *Rmax, int *Rav);
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// r here is the radius of gyration
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *r);
	inline void fitCircle(YARPImageOf<YarpPixelMono> &in, YARPShapeCircle &circle);
	// fit an ellipse to the segmented region. a11, a12, a22 are the parameters of the conic matrix
	// computed from the central moments (see code for the formulas)
	void fitEllipse(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, double *a11, double *a12, double *a22);
	inline void fitEllipse(YARPImageOf<YarpPixelMono> &in, YARPShapeEllipse &el);

	// returns points within a circle of center T0,R0 radius R0
	void findCircle(int T0, int R0, double R, YARPShapeRegion &out);
	inline void findCircle(const YARPShapeCircle &c, YARPShapeRegion &out);
	// returns points within an ellipse
	void findEllipse(int T0, int R0, double a11, double a12, double a22, YARPShapeRegion &out);
	inline void findEllipse(const YARPShapeEllipse &el, YARPShapeRegion &out);

	// plot a circle; 
	// (T0, R0) is the center (logpolar coordinates), R is the radius (cartesian coordinates)
	void plotCircle(int t0, int r0, double R, YARPImageOf<YarpPixelMono> &output);
	inline void plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelMono> &output);
	// plot an ellipse from the coefficients of the quadratic form
	// (T0, R0) is the center (logpolar coordinates)
	void plotEllipse(int t0, int r0, double a11, double a12, double a22, YARPImageOf<YarpPixelMono> &output);
	inline void plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output);

private:
	// compute minimum, maximum and average radius of the seg region
	void _radius(YARPImageOf<YarpPixelMono> &in, int theta,  int rho, int *Rmin, int *Rmax, int *Rav);
	// compute the radius of gyration of the segmented region
	double _radiusOfGyration(YARPImageOf<YarpPixelMono> &in, int x, int y);

	YARPLpImageMoments _moments;
};

// inlines
inline void YARPLpConicFitter::plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelMono> &output)
{
	plotCircle(circle.theta, circle.rho, circle.radius, output);
}

inline void YARPLpConicFitter::plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output)
{
	plotEllipse(ellipse.theta,
				ellipse.rho,
				ellipse.a11,
				ellipse.a12,
				ellipse.a22, output);
}

inline void YARPLpConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, YARPShapeCircle &circle)
{
	fitCircle(in, &circle.theta, &circle.rho, &circle.radius);
}

inline void YARPLpConicFitter::fitEllipse(YARPImageOf<YarpPixelMono> &in, YARPShapeEllipse &el)
{
	fitEllipse(in, &el.theta, &el.rho, &el.a11, &el.a12, &el.a22);
}

inline void YARPLpConicFitter::findCircle(const YARPShapeCircle &circle, YARPShapeRegion &out)
{
	findCircle(circle.theta, circle.rho, circle.radius, out);
}

inline void YARPLpConicFitter::findEllipse(const YARPShapeEllipse &el, YARPShapeRegion &out)
{
	findEllipse(el.theta, el.rho, el.a11, el.a12, el.a22, out);
}

#endif
