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
/// $Id: YARPLogpolar.cpp,v 1.1 2003-06-10 15:49:49 babybot Exp $
///
///

#include "YARPLogpolar.h"


YARPLogpolar::YARPLogpolar (void)
{
	using namespace _logpolarParams;

	_fovea = Set_Param(
		_xsize, _ysize,
		128, 128,
		_srho, _stheta, _sfovea,
		1090,
		CUSTOM,
		256.0/1090.0);

	_periphery = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho, _stheta, _sfovea,
		1090,
		CUSTOM,
		256.0/1090.0);


	char path[256];
	/// get path from env variable.

	/// load cart to logpolar lookup table.
	_cart2LP_Map = Load_Cart2LP_Map(&_periphery, path);

	/// logpolar to cartesian lookup table for the fovea.
	char filename[256];
	ACE_OS::sprintf(filename, "%s%s_%2.3f_%dx%d%s", path,"RemapMap", _fovea.Zoom_Level, _fovea.Size_X_Remap, _fovea.Size_Y_Remap, ".gio");

	FILE *fin;
	fin = ACE_OS::fopen(filename,"rb");
	ACE_ASSERT (fin != NULL);

	_remapMap = (int *) malloc (_fovea.Size_Img_Remap * sizeof(int));
	ACE_ASSERT (_remapMap != NULL);

	ACE_OS::fread(_remapMap, sizeof(int), _fovea.Size_Img_Remap, fin);
	ACE_OS::fclose (fin);

	/// logpolar to cartesian lut for the periphery.
	ACE_OS::sprintf(filename,"%s%s_%2.3f%s", path, "RemapMapNoFov", _periphery.Zoom_Level, ".gio");
	fin = ACE_OS::fopen(filename,"rb");
	ACE_ASSERT (fin != NULL);

	_remapMapNf = (int *) malloc (_periphery.Size_Img_Remap * sizeof(int));
	ACE_ASSERT (_remapMapNf != NULL);

	ACE_OS::fread(_remapMapNf, sizeof(int), _periphery.Size_Img_Remap, fin);
	ACE_OS::fclose (fin);
}

YARPLogpolar::~YARPLogpolar ()
{
	if (_remapMap != NULL) free (_remapMap);
}

