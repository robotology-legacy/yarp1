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
/// $Id: YARPLogpolar.h,v 1.10 2003-08-13 00:23:18 gmetta Exp $
///
///

#ifndef __YARPLogpolarh__
#define __YARPLogpolarh__

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#ifdef YARP_HAS_PRAGMA_ONCE
#	pragma once
#endif

#include <YARPImages.h>
#include <LogPolarSDK.h>

///
/// embeds the logpolar library for YARP. The librery is a relatively accurate simulation
///		of one of the latest logpolar CMOS chips.
///

namespace _logpolarParams
{
	const int _xsize = 256;
	const int _ysize = 256;
	const int _srho = 152;
	const int _stheta = 252;
	const int _sfovea = 42;
};


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
	Image_Data _img;
	int * _remapMap;
	int * _remapMapFovea;
	double *_angShiftMap;
	short *_padMap;
	Neighborhood * _weightsMap;
	bool _mapsLoaded;

public:
	YARPLogpolar (void);
	virtual ~YARPLogpolar ();

	virtual void Cleanup () {}
	virtual bool InPlace () const { return false; }

	int Logpolar2Cartesian (const YARPGenericImage& in, YARPGenericImage& out);
	int Logpolar2CartesianFovea (const YARPGenericImage& in, YARPGenericImage& out);
	int Logpolar2Cartesian (int irho, int itheta, int& ox, int& oy);
	int Cartesian2Logpolar (int ix, int iy, int& orho, int& otheta);
	int ReconstructColor (const YARPImageOf<YarpPixelMono>& in, YARPGenericImage& out);
	int ReconstructGrays (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out);

	inline int GetCWidth (void) const { return _logpolarParams::_xsize; }
	inline int GetCHeight (void) const { return _logpolarParams::_ysize; }

	bool TablesOk(void) const { return _mapsLoaded; }
};


#endif

