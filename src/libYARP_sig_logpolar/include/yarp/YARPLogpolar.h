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
///                    #pasa & nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPLogpolar.h,v 1.3 2004-07-20 15:00:57 babybot Exp $
///
///

#ifndef __YARPLogpolarh__
#define __YARPLogpolarh__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Synch.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <yarp/YARPImages.h>
#include <yarp/LogPolarSDK.h>

///
/// embeds the logpolar library for YARP. The librery is a relatively accurate simulation
///	of one of the latest logpolar CMOS chips.
///

// Logpolar trans -- Notational conventions:
// x = ro*cos(eta/q)
// y = ro*sin(eta/q)
// ro = k1 + k2*lambda^(csi) csi > F
// r0 = csi					 csi <= F
// J(csi, eta) = lambda^csi * ln(lambda)/q (k1+k2*lambda^csi) outside the fovea
// J(csi, eta) = J(F, eta) within the fovea
// Jan 2004 -- by nat

namespace _logpolarParams
{
	const int _xsize = 256;
	const int _ysize = 256;
	const int _srho = 152;
	const int _stheta = 252;
	const int _sfovea = 42;
	
	const int _xsizefovea = 128;
	const int _ysizefovea = 128;
	// this is the ratio between the full size cartesian image and the actual one
	const double _ratio = 0.25;		// 1/4
	
	// parameter of the transformation
	const double _q = _stheta/(2*PI);
	const double _lambda = 1.02314422608633;
	const double _logLambda = log(_lambda);
	const double _k1 = (_sfovea - 0.5)+(_lambda)/(1-_lambda);
	const double _k2 = _lambda/(pow(_lambda,_sfovea)*(_lambda-1));
};

// NOTE: this function was determined empirically and it is far from being correct
// still used within YARPBlobDetector class
inline double pSize(int c, int r, int nf = _logpolarParams::_sfovea)
{
	ACE_UNUSED_ARG(c);

	const double _alfa = 0.0130;
	const double _beta = 0.91;

	double ret;
	ret = _beta*exp(_alfa*(r-nf));
	// ret = _alfa*(r-nf) + _beta;
	if (ret < 1)
		ret = 1;
	
	return ret;
}

class YARPLogpolarSampler : public YARPFilter
{
protected:
	Image_Data _img;
	Cart2LPInterp * _cart2LP_Map;
	unsigned char * _outimage;
	char * _colormap;
	int _padb;

public:
	YARPLogpolarSampler (void);
	virtual ~YARPLogpolarSampler ();

	virtual void Cleanup () {}
	virtual bool InPlace () const { return false; }

	int Cartesian2Logpolar (const YARPGenericImage& in, YARPGenericImage& out);
};


class YARPLogpolar : public YARPFilter
{
protected:
	static ACE_Thread_Mutex _mutex;
	static Image_Data _img;
	static int * _remapMap;
	static int * _remapMapFovea;
	static double *_angShiftMap;
	static unsigned short *_padMap;
	static unsigned int _classInstances;
	static Neighborhood * _weightsMap;
	bool _mapsLoaded;

public:
	YARPLogpolar (void);
	virtual ~YARPLogpolar ();

	virtual void Cleanup () {}
	virtual bool InPlace () const { return false; }

	int Logpolar2Cartesian (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out);
	int Logpolar2CartesianFovea (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out);
	int Logpolar2Cartesian (int irho, int itheta, int& ox, int& oy);
	int Cartesian2Logpolar (double ix, double iy, int& orho, int& otheta);
	inline int Cartesian2Logpolar (int ix, int iy, int& orho, int& otheta) { return Cartesian2Logpolar ((double) ix, (double) iy, orho, otheta); }
	int ReconstructColor (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out);
	int ReconstructGrays (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out);
	int Sawt2Uniform(const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out);
	int Uniform2Sawt(const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out);

	// slightly slower function which internally check images are allocated
	int SafeLogpolar2Cartesian (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
	{ 	
		out.CastID(YARP_PIXEL_BGR);
		out.Resize(_logpolarParams::_xsize, _logpolarParams::_ysize);
		return Logpolar2Cartesian (in, out);
	}

	int SafeLogpolar2CartesianFovea (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
	{
		out.CastID(YARP_PIXEL_BGR);
		out.Resize(_logpolarParams::_xsizefovea, _logpolarParams::_ysizefovea);
		return Logpolar2CartesianFovea(in, out);
	}
	int SafeReconstructColor (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR>& out)
	{
		out.CastID(YARP_PIXEL_BGR);
		out.Resize(in.GetWidth(), in.GetHeight());
		return ReconstructColor(in, out);
	}

	int SafeReconstructGrays (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out)
	{
		out.CastID(YARP_PIXEL_MONO);
		out.Resize(in.GetWidth(), in.GetHeight());
		return ReconstructGrays(in, out);
	}
	
	inline int GetCWidth (void) const { return _logpolarParams::_xsize; }
	inline int GetCHeight (void) const { return _logpolarParams::_ysize; }

	inline double CsiToRo(double csi);
	inline int RoToCsi(double r);
	inline double Jacobian(int csi, int eta);
	
	bool TablesOk(void) const { return _mapsLoaded; }
};

inline double YARPLogpolar::CsiToRo(double csi)
{
	const double k1 = _logpolarParams::_k1;
	const double k2 = _logpolarParams::_k2;
	const double lambda = _logpolarParams::_lambda;

	double r;

	if (csi <= _logpolarParams::_sfovea)
		r = (csi-0.5);
	else
		r = (k1+k2*pow(lambda, csi));
			
	return r*_logpolarParams::_ratio;
}

inline int YARPLogpolar::RoToCsi(double r)
{
	const double k1 = _logpolarParams::_k1;
	const double k2 = _logpolarParams::_k2;
	const double lambda = _logpolarParams::_lambda;

	double csi;
	double tmpR = r/_logpolarParams::_ratio;

	if (tmpR <= _logpolarParams::_sfovea)
		csi = tmpR;
	else
	{
		double tmp1 = (tmpR-k1)/k2;	// always > 1 out of the fovea
		csi = (1/log(lambda))*log(tmp1);
	}	
	
	return (int) csi;
}

// compute the jacobian of the transformation
inline double YARPLogpolar::Jacobian(int csi, int eta)
{
	ACE_UNUSED_ARG(eta);	// keep the compiler happy
	const double logLambda = _logpolarParams::_logLambda;
	const double lambda = _logpolarParams::_lambda;
	const double q = _logpolarParams::_q;
	const double k1 = _logpolarParams::_k1;
	const double k2 = _logpolarParams::_k2;
	
	double ret = 0.0;
	double c;

	if (csi >= _logpolarParams::_sfovea)
		c = csi;
	else
		c = _logpolarParams::_sfovea;
	
	double tmpPow = pow(lambda, c);
	ret = logLambda*tmpPow*(k1 + k2*tmpPow)/q;

	return (ret);
}

#endif

