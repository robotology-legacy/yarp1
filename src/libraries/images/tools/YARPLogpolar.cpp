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
/// $Id: YARPLogpolar.cpp,v 1.6 2003-06-16 16:48:24 babybot Exp $
///
///

#include "YARPLogpolar.h"
#include "YARPAll.h"


YARPLogpolarSampler::YARPLogpolarSampler (void)
{
	using namespace _logpolarParams;

	_fovea = Set_Param(
		_xsize, _ysize,
		128, 128,
		_srho, _stheta, _sfovea,
		1090,
		CUSTOM,
		256.0/1090.0);

	_fovea.padding = 4;

	char *path = GetYarpRoot ();
	char filename[256];

	ACE_OS::sprintf(filename, "%s\\conf\\\0", path);
	/// loads cart to logpolar lookup table.
	_cart2LP_Map = Load_Cart2LP_Map(&_fovea, filename);

	/// logpolar to cartesian lookup table for the fovea.
	ACE_OS::sprintf(filename, "%s\\conf\\%s_%2.3f_%dx%d%s", path, "RemapMap", _fovea.Zoom_Level, _fovea.Size_X_Remap, _fovea.Size_Y_Remap, ".gio");

	FILE *fin;
	fin = ACE_OS::fopen(filename,"rb");
	ACE_ASSERT (fin != NULL);

	_remapMap = (int *) malloc (_fovea.Size_Img_Remap * sizeof(int));
	ACE_ASSERT (_remapMap != NULL);

	ACE_OS::fread(_remapMap, sizeof(int), _fovea.Size_Img_Remap, fin);
	ACE_OS::fclose (fin);

	_outimage = (unsigned char *)malloc (_srho * _stheta * 3);
	ACE_ASSERT (_outimage != NULL);
}

YARPLogpolarSampler::~YARPLogpolarSampler ()
{
	Free_Cart2LP_Map (_cart2LP_Map);

	if (_remapMap != NULL) free (_remapMap);
	if (_outimage != NULL) free (_outimage);
}

int YARPLogpolarSampler::Cartesian2Logpolar (const YARPGenericImage& in, YARPGenericImage& fovea, YARPGenericImage& periphery)
{
	using namespace _logpolarParams;
	ACE_ASSERT (in.GetWidth() == _xsize && in.GetHeight() == _ysize);
	ACE_ASSERT (fovea.GetWidth() == 128 && fovea.GetHeight() == 128);
	ACE_ASSERT (periphery.GetWidth() == _stheta && periphery.GetHeight() == (_srho - _sfovea));

	Make_LP_Real (_outimage, (unsigned char *)in.GetRawBuffer(), &_fovea, _cart2LP_Map);
	
	memcpy (periphery.GetRawBuffer(), _outimage + _sfovea * periphery.GetAllocatedLineSize(), (_srho - _sfovea) * periphery.GetAllocatedLineSize());

	Remap ((unsigned char *)fovea.GetRawBuffer(), _outimage, &_fovea, _remapMap);

	return YARP_OK;
}

///
///
///
YARPLogpolar::YARPLogpolar (void)
{
	using namespace _logpolarParams;

	_periphery = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho, _stheta, _sfovea,
		1090,
		CUSTOM,
		256.0/1090.0);

	_periphery.padding = 4;

	char *path = GetYarpRoot ();

	/// logpolar to cartesian lookup table for the fovea.
	char filename[256];

	/// logpolar to cartesian lut for the periphery.
	ACE_OS::sprintf(filename,"%s\\conf\\%s_%2.3f%s", path, "RemapMapNoFov", _periphery.Zoom_Level, ".gio");
	FILE *fin = ACE_OS::fopen(filename,"rb");
	ACE_ASSERT (fin != NULL);

	_remapMapNf = (int *) malloc (_periphery.Size_Img_Remap * sizeof(int));
	ACE_ASSERT (_remapMapNf != NULL);

	ACE_OS::fread(_remapMapNf, sizeof(int), _periphery.Size_Img_Remap, fin);
	ACE_OS::fclose (fin);

	ACE_OS::sprintf(filename, "%s\\%s", path, "AngularShiftMap.gio");
	fin = ACE_OS::fopen(filename, "rb");
	ACE_ASSERT (fin != NULL);

	_angShiftMap = (double *) malloc (_periphery.Size_Rho * sizeof(double));
	ACE_ASSERT (_angShiftMap != NULL);
	ACE_OS::fread(_angShiftMap, sizeof(double), _periphery.Size_Rho, fin);
	ACE_OS::fclose (fin);

	ACE_OS::sprintf(filename, "%s\\%s", path, "PadMap.gio");
	fin = ACE_OS::fopen(filename, "rb");
	ACE_ASSERT (fin != NULL);

	_padMap = (short *) malloc (_periphery.Size_Theta * _periphery.Size_Fovea * sizeof(short));
	ACE_ASSERT (_padMap != NULL);

	ACE_OS::fread(_padMap, sizeof(short), _periphery.Size_Theta * _periphery.Size_Fovea, fin);
	ACE_OS::fclose (fin);
}

YARPLogpolar::~YARPLogpolar ()
{
	if (_remapMapNf != NULL) free (_remapMapNf);
	if (_angShiftMap != NULL) free (_angShiftMap);
	if (_padMap != NULL) free (_padMap);
}

int YARPLogpolar::Logpolar2Cartesian (int irho, int itheta, int& ox, int& oy)
{
	double xx = 0;
	double yy = 0;

	Get_XY_Center(&xx, &yy, irho, itheta, &_periphery, _angShiftMap);

	ox = int(xx + .5);
	oy = int(yy + .5);

	return YARP_OK;
}

/// out -> 256 x 256, in 110 x 256.
///
int YARPLogpolar::Logpolar2Cartesian (const YARPGenericImage& in, YARPGenericImage& out)
{
	using namespace _logpolarParams;
	ACE_ASSERT (in.GetWidth() == 256 && in.GetHeight() == _srho - _sfovea);
	ACE_ASSERT (in.GetWidth() == _xsize && in.GetHeight() == _ysize);

	Remap ((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_periphery, _remapMapNf);
	return YARP_OK;
}

int YARPLogpolar::Cartesian2Logpolar (int ix, int iy, int& orho, int& otheta)
{
	orho = Get_Rho (ix, iy, &_periphery);
	otheta = Get_Theta(ix, iy, orho, &_periphery, _angShiftMap, _padMap);

	return YARP_OK;
}

