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
/// $Id: YARPConicFitter.h,v 1.10 2004-04-21 17:53:15 natta Exp $
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
	YARPShapeCircle(){x = 0; y = 0; radius = 0;};

	int x;
	int y;
	int radius;
};

class YARPShapeEllipse
{
public:
	YARPShapeEllipse()
	{ x = 0; y = 0; a11 = 0.0; a12 = 0.0; a22 = 0.0; a = 0.0; b = 0.0; angle = 0.0;};
	~YARPShapeEllipse(){};

	void scale(double sc)
	{
		x = x/sc;
		y = y/sc;
		a11 = a11*(sc*sc);
		a12 = a12*(sc*sc);
		a22 = a22*(sc*sc);

		a = a/sc;
		b = b/sc;
	}

	int x;
	int y;
	double a11;
	double a22;
	double a12;

	double a;
	double b;
	double angle;
};

class YARPLpShapeEllipse: public YARPShapeEllipse
{
public:
	YARPLpShapeEllipse()
	{ theta = 0; rho = 0; }
	~YARPLpShapeEllipse(){};

	int theta;
	int rho;
};

class YARPLpShapeCircle
{
public:
	YARPLpShapeCircle()
	{ theta = 0; rho = 0; radius = 0;};
	~YARPLpShapeCircle(){};

	int theta;
	int rho;
	int radius;
};

class YARPShapeRegion
{
public:
	YARPShapeRegion()
	{
		x = new int [_logpolarParams::_xsize*_logpolarParams::_ysize];
		y = new int [_logpolarParams::_xsize*_logpolarParams::_ysize];
	}
	~YARPShapeRegion()
	{
		delete [] x;
		delete [] y;
	}

	void reset()
	{ n = 0; }

	void add(int X, int Y)
	{ 
		x[n] = X;
		y[n] = Y;
		n++;
	}

	int *x;
	int *y;
	int n;
};

class YARPLpShapeRegion
{
public:
	YARPLpShapeRegion()
	{
		r = new int [_logpolarParams::_srho*_logpolarParams::_stheta];
		t = new int [_logpolarParams::_srho*_logpolarParams::_stheta];
	}
	~YARPLpShapeRegion()
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

///////////////////// LOG POLAR
class YARPLpConicFitter
{
public:
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// Rmin, Rmax, Rav are respectively min, max and av radius of the region.
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *Rmin, int *Rmax, int *Rav);
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// r here is the radius of gyration
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, int *r);
	inline void fitCircle(YARPImageOf<YarpPixelMono> &in, YARPLpShapeCircle &circle);
	// fit an ellipse to the segmented region. a11, a12, a22 are the parameters of the conic matrix
	// computed from the central moments (see code for the formulas)
	inline void fitEllipse(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, double *a11, double *a12, double *a22);
	void fitEllipse(YARPImageOf<YarpPixelMono> &in, YARPLpShapeEllipse &el);

	// returns points within a circle of center T0,R0 radius R0
	void findCircle(int T0, int R0, double R, YARPLpShapeRegion &out);
	inline void findCircle(const YARPLpShapeCircle &c, YARPLpShapeRegion &out);
	// returns points within an ellipse
	void findEllipse(int T0, int R0, double a11, double a12, double a22, YARPLpShapeRegion &out);
	inline void findEllipse(const YARPLpShapeEllipse &el, YARPLpShapeRegion &out);

	// plot a circle; 
	// (T0, R0) is the center (logpolar coordinates), R is the radius (cartesian coordinates)
	void plotCircle(int t0, int r0, double R, YARPImageOf<YarpPixelMono> &output);
	inline void plotCircle(const YARPLpShapeCircle &circle, YARPImageOf<YarpPixelMono> &output);
	void plotCircle(int t0, int r0, double R, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));
	inline void plotCircle(const YARPLpShapeCircle &circle, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));

	// plot an ellipse from the coefficients of the quadratic form
	// (T0, R0) is the center (logpolar coordinates)
	void plotEllipse(int t0, int r0, double a11, double a12, double a22, YARPImageOf<YarpPixelMono> &output);
	inline void plotEllipse(const YARPLpShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output);
	
	void plotEllipse(int T0, int R0, double a11, double a12, double a22, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));
	inline void plotEllipse(const YARPLpShapeEllipse &ellipse, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));

private:
	// compute minimum, maximum and average radius of the seg region
	void _radius(YARPImageOf<YarpPixelMono> &in, int theta,  int rho, int *Rmin, int *Rmax, int *Rav);
	// compute the radius of gyration of the segmented region
	double _radiusOfGyration(YARPImageOf<YarpPixelMono> &in, int x, int y);
	inline double _det(double a11, double a12, double a22)
	{	return (a11*a22 - a12*a12); }
	inline bool _checkDet(double a11, double a12, double a22)
	{	return (_det(a11, a12, a22) > 0.0); }

	YARPLpImageMoments _moments;
};

// inlines
inline void YARPLpConicFitter::plotCircle(const YARPLpShapeCircle &circle, YARPImageOf<YarpPixelMono> &output)
{
	plotCircle(circle.theta, circle.rho, circle.radius, output);
}

inline void YARPLpConicFitter::plotCircle(const YARPLpShapeCircle &circle, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	plotCircle(circle.theta, circle.rho, circle.radius, output, v);
}

inline void YARPLpConicFitter::plotEllipse(const YARPLpShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output)
{
	plotEllipse(ellipse.theta,
				ellipse.rho,
				ellipse.a11,
				ellipse.a12,
				ellipse.a22, output);
}

inline void YARPLpConicFitter::plotEllipse(const YARPLpShapeEllipse &ellipse, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	plotEllipse(ellipse.theta,
				ellipse.rho,
				ellipse.a11,
				ellipse.a12,
				ellipse.a22,
				output,
				v);
}

inline void YARPLpConicFitter::fitEllipse(YARPImageOf<YarpPixelMono> &in, int *t0,  int *r0, double *a11, double *a12, double *a22)
{
	YARPLpShapeEllipse el;

	fitEllipse(in, el);

	*t0 = el.theta;
	*r0 = el.rho;

	*a11 = el.a11;
	*a12 = el.a12;
	*a22 = el.a22;
}

inline void YARPLpConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, YARPLpShapeCircle &circle)
{
	fitCircle(in, &circle.theta, &circle.rho, &circle.radius);
}

inline void YARPLpConicFitter::findCircle(const YARPLpShapeCircle &circle, YARPLpShapeRegion &out)
{
	findCircle(circle.theta, circle.rho, circle.radius, out);
}

inline void YARPLpConicFitter::findEllipse(const YARPLpShapeEllipse &el, YARPLpShapeRegion &out)
{
	findEllipse(el.theta, el.rho, el.a11, el.a12, el.a22, out);
}

////////////// CARTESIAN
class YARPConicFitter
{
public:
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// Rmin, Rmax, Rav are respectively min, max and av radius of the region.
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *x,  int *y, int *Rmin, int *Rmax, int *Rav);
	// fit a circle to a segmented region; r0, t0 is the center of the circle (center of mass)
	// r here is the radius of gyration
	void fitCircle(YARPImageOf<YarpPixelMono> &in, int *x,  int *y, int *r);
	inline void fitCircle(YARPImageOf<YarpPixelMono> &in, YARPShapeCircle &circle);
	// fit an ellipse to the segmented region. a11, a12, a22 are the parameters of the conic matrix
	// computed from the central moments (see code for the formulas)
	inline void fitEllipse(YARPImageOf<YarpPixelMono> &in, int *x,  int *y, double *a11, double *a12, double *a22);
	void fitEllipse(YARPImageOf<YarpPixelMono> &in, YARPShapeEllipse &el);

	// returns points within a circle of center x0,y0 radius R0
	void findCircle(int x0, int y0, double R, YARPShapeRegion &out);
	inline void findCircle(const YARPShapeCircle &c, YARPShapeRegion &out);
	// returns points within an ellipse
	void findEllipse(int x0, int y0, double a11, double a12, double a22, YARPShapeRegion &out);
	inline void findEllipse(const YARPShapeEllipse &el, YARPShapeRegion &out);

	// plot a circle; 
	// (X0, Y0) is the center (logpolar coordinates), R is the radius (cartesian coordinates)
	void plotCircle(int x0, int y0, double R, YARPImageOf<YarpPixelMono> &output);
	inline void plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelMono> &output);
	void plotCircle(int x0, int y0, double R, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));
	inline void plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));

	// plot an ellipse from the coefficients of the quadratic form
	// (X0, Y0) is the center (logpolar coordinates)
	void plotEllipse(int x0, int y0, double a11, double a12, double a22, YARPImageOf<YarpPixelMono> &output);
	inline void plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output);
	
	void plotEllipse(int x0, int y0, double a11, double a12, double a22, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));
	inline void plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR&v = YarpPixelBGR(255, 0, 0));

private:
	// compute minimum, maximum and average radius of the seg region
	void _radius(YARPImageOf<YarpPixelMono> &in, int x0,  int y0, int *Rmin, int *Rmax, int *Rav);
	// compute the radius of gyration of the segmented region
	double _radiusOfGyration(YARPImageOf<YarpPixelMono> &in, int x, int y);
	inline double _det(double a11, double a12, double a22)
	{	return (a11*a22 - a12*a12); }
	inline bool _checkDet(double a11, double a12, double a22)
	{	return (_det(a11, a12, a22) > 0.0); }

	YARPImageMoments _moments;
};

// inlines
inline void YARPConicFitter::plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelMono> &output)
{
	plotCircle(circle.x, circle.y, circle.radius, output);
}

inline void YARPConicFitter::plotCircle(const YARPShapeCircle &circle, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	plotCircle(circle.x, circle.y, circle.radius, output, v);
}

inline void YARPConicFitter::plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelMono> &output)
{
	plotEllipse(ellipse.x,
				ellipse.y,
				ellipse.a11,
				ellipse.a12,
				ellipse.a22, output);
}

inline void YARPConicFitter::plotEllipse(const YARPShapeEllipse &ellipse, YARPImageOf<YarpPixelBGR> &output, const YarpPixelBGR &v)
{
	plotEllipse(ellipse.x,
				ellipse.y,
				ellipse.a11,
				ellipse.a12,
				ellipse.a22,
				output,
				v);
}

inline void YARPConicFitter::fitCircle(YARPImageOf<YarpPixelMono> &in, YARPShapeCircle &circle)
{
	fitCircle(in, &circle.x, &circle.y, &circle.radius);
}

inline void YARPConicFitter::fitEllipse(YARPImageOf<YarpPixelMono> &in, int *x,  int *y, double *a11, double *a12, double *a22)
{
	YARPShapeEllipse el;
	fitEllipse(in, el);

	*a11 = el.a11;
	*a12 = el.a12;
	*a22 = el.a22;

	*x = el.x;
	*y = el.y;
}

inline void YARPConicFitter::findCircle(const YARPShapeCircle &circle, YARPShapeRegion &out)
{
	findCircle(circle.x, circle.y, circle.radius, out);
}

inline void YARPConicFitter::findEllipse(const YARPShapeEllipse &el, YARPShapeRegion &out)
{
	findEllipse(el.x, el.y, el.a11, el.a12, el.a22, out);
}

#endif
