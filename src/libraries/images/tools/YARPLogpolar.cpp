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
/// $Id: YARPLogpolar.cpp,v 1.26 2004-04-26 17:15:37 natta Exp $
///
///

#include "YARPLogpolar.h"
#include "YARPAll.h"
#include <ace/Synch.h>

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif


YARPLogpolarSampler::YARPLogpolarSampler (void)
{
	using namespace _logpolarParams;

	_img = Set_Param(
		_xsize, _ysize,
		256, 256,
		_srho, _stheta, _sfovea,
		1090,
		CUST,
		256.0/1090.0);

	_img.padding = YarpImageAlign;
	_img.Fovea_Type = 0;

	char *path = GetYarpRoot ();
	char filename[256];

	ACE_OS::sprintf(filename, "%s/conf/\0", path);

	/// loads cart to logpolar lookup table.
	_cart2LP_Map = Load_Cart2LP_Map(&_img, filename);

	_colormap = (char *) malloc (_srho * _stheta);
	ACE_ASSERT (_colormap != NULL);

	ACE_OS::sprintf(filename, "%s/conf/ColorMap.gio\0", path);

	FILE *fin;

	fin = ACE_OS::fopen(filename,"rb");
	ACE_ASSERT (fin != NULL);
	ACE_OS::fread(_colormap, sizeof(char), _srho * _stheta, fin);
	ACE_OS::fclose (fin);

	_padb = YARPSimpleOperation::ComputePadding (_stheta * 3, _img.padding);
	_outimage = (unsigned char *)malloc (_srho * (_stheta * 3 + _padb));
	ACE_ASSERT (_outimage != NULL);
}

YARPLogpolarSampler::~YARPLogpolarSampler ()
{
	Free_Cart2LP_Map (_cart2LP_Map);

	if (_outimage != NULL) free (_outimage);
	if (_colormap != NULL) free (_colormap);
}

int YARPLogpolarSampler::Cartesian2Logpolar (const YARPGenericImage& in, YARPGenericImage& out)
{
	using namespace _logpolarParams;
	ACE_ASSERT (in.GetWidth() == _xsize && in.GetHeight() == _ysize);
	ACE_ASSERT (out.GetWidth() == _stheta && out.GetHeight() == _srho);

	Make_LP_Real (_outimage, (unsigned char *)in.GetRawBuffer(), &_img, _cart2LP_Map);
	
	char *de = out.GetRawBuffer();
	unsigned char * o = _outimage;
	char *cmap = _colormap;
	const int w = out.GetWidth();
	const int h = out.GetHeight();
	const int p = out.GetPadding();

	int i, j;
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w; j++)
		{
			*de++ = *(o + *cmap++); 
			o += 3;
		}

		de += p;
		o += _padb;
	}

	return YARP_OK;
}

///
///
/// YARPLogpolar, static variables
unsigned int YARPLogpolar::_classInstances = 0;
int *YARPLogpolar::_remapMap = NULL;
int *YARPLogpolar::_remapMapFovea = NULL;
double *YARPLogpolar::_angShiftMap = NULL;
unsigned short *YARPLogpolar::_padMap = NULL;
Neighborhood *YARPLogpolar::_weightsMap = NULL;
Image_Data YARPLogpolar::_img;
ACE_Thread_Mutex YARPLogpolar::_mutex;

YARPLogpolar::YARPLogpolar (void)
{
	using namespace _logpolarParams;

	_mapsLoaded = true;
	
	ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
	{	
		// begin critical section
		if (_classInstances == 0)
		{
			_img = Set_Param(
				_xsize, _ysize,
				256, 256,
				_srho, _stheta, _sfovea,
				1090,
				CUST,
				256.0/1090.0);

			_img.padding = YarpImageAlign;
			_img.Pix_Numb = 2;
			_img.Fovea_Type = 0;

			char *path = GetYarpRoot ();

			/// logpolar to cartesian lookup table for the complete image.
			char filename[YARP_STRING_LEN];
			ACE_OS::sprintf(filename,"%s/conf/%s_%2.3f_%dx%d%s%d%s", path, "RemapMap", _img.Zoom_Level, _img.Size_X_Remap, _img.Size_Y_Remap, "_P", _img.padding, ".gio");

			FILE *fin = ACE_OS::fopen(filename,"rb");
			if (fin == NULL)
				goto exitConstructorOnError;

			_remapMap = (int *) malloc (_img.Size_Img_Remap * sizeof(int));
			ACE_ASSERT (_remapMap != NULL);

			ACE_OS::fread(_remapMap, sizeof(int), _img.Size_Img_Remap, fin);
			ACE_OS::fclose (fin);

			ACE_OS::sprintf(filename, "%s/conf/%s", path, "AngularShiftMap.gio");

			fin = ACE_OS::fopen(filename, "rb");
			if (fin == NULL)
				goto exitConstructorOnError;
	
			_angShiftMap = (double *) malloc (_img.Size_Rho * sizeof(double));
			ACE_ASSERT (_angShiftMap != NULL);
			ACE_OS::fread(_angShiftMap, sizeof(double), _img.Size_Rho, fin);
			ACE_OS::fclose (fin);

			ACE_OS::sprintf(filename, "%s/conf/%s", path, "PadMap.gio");
			fin = ACE_OS::fopen(filename, "rb");
			if (fin == NULL)
				goto exitConstructorOnError;

			_padMap = (unsigned short *) malloc (_img.Size_Theta * _img.Size_Fovea * sizeof(unsigned short));	
			ACE_ASSERT (_padMap != NULL);

			ACE_OS::fread(_padMap, sizeof(short), _img.Size_Theta * _img.Size_Fovea, fin);
			ACE_OS::fclose (fin);

			ACE_OS::sprintf(filename, "%s/conf/%s%02d%s%d%s", path, "WeightsMap", _img.Pix_Numb, "_P", _img.padding, ".gio");
			fin = ACE_OS::fopen(filename, "rb");
			if (fin == NULL)
				goto exitConstructorOnError;

			_weightsMap = (Neighborhood *) malloc (_img.Size_LP * _img.Pix_Numb * 3 * sizeof(Neighborhood));
			ACE_ASSERT (_weightsMap != NULL);

			ACE_OS::fread(_weightsMap, sizeof(Neighborhood), _img.Size_LP * _img.Pix_Numb * 3, fin);
			ACE_OS::fclose (fin);

			_img = Set_Param(
				_xsize, _ysize,
				_xsizefovea, _ysizefovea,
				_srho, _stheta, _sfovea,
				1090,
				CUST,
				512.0/1090.0);

			_img.padding = YarpImageAlign;
			_img.Pix_Numb = 2;
			_img.Fovea_Type = 0;

			/// remap lut for the fovea.
			ACE_OS::sprintf(filename,"%s/conf/%s_%2.3f_%dx%d%s%d%s", path, "RemapMap", _img.Zoom_Level, _img.Size_X_Remap, _img.Size_Y_Remap, "_P", _img.padding, ".gio");
			fin = ACE_OS::fopen(filename,"rb");
			if (fin == NULL)
				goto exitConstructorOnError;

			_remapMapFovea = (int *) malloc (_img.Size_Img_Remap * sizeof(int));
			ACE_ASSERT (_remapMapFovea != NULL);

			ACE_OS::fread(_remapMapFovea, sizeof(int), _img.Size_Img_Remap, fin);
			ACE_OS::fclose (fin);

			_img = Set_Param(
				_xsize, _ysize,
				256, 256,
				_srho, _stheta, _sfovea,
				1090,
				CUST,
				256.0/1090.0);
	
			_img.padding = YarpImageAlign;
			_img.Pix_Numb = 2;
			_img.Fovea_Type = 0;
		}
	
		// everything went fine, increment instance counter
		_classInstances++;
	}	// end critical section

	return;

exitConstructorOnError:
	_mapsLoaded = false;

	if (_remapMap != NULL) free (_remapMap);
	_remapMap = NULL;
	if (_remapMapFovea != NULL) free (_remapMapFovea);
	_remapMapFovea = NULL;
	if (_angShiftMap != NULL) free (_angShiftMap);
	_angShiftMap = NULL;
	if (_padMap != NULL) free (_padMap);
	_padMap = NULL;
	if (_weightsMap != NULL) free (_weightsMap);
	_weightsMap = NULL;
}

YARPLogpolar::~YARPLogpolar ()
{
	ACE_Guard<ACE_Thread_Mutex> guard(_mutex);
	{ 
		// begin critical section
		_classInstances--;
		if (_classInstances <= 0)
		{
			if (_remapMap != NULL) free (_remapMap);
			if (_remapMapFovea != NULL) free (_remapMapFovea);
	
			if (_angShiftMap != NULL) free (_angShiftMap);
			if (_padMap != NULL) free (_padMap);
			if (_weightsMap != NULL) free (_weightsMap);

		}
	} //  end critical section
}

///
///
/// ox, oy in [0,max]x[0,max]
int YARPLogpolar::Logpolar2Cartesian (int irho, int itheta, int& ox, int& oy)
{
	double xx = 0;
	double yy = 0;

	Get_XY_Center(&xx, &yy, irho, itheta, &_img, _angShiftMap);

	using namespace _logpolarParams;
	ox = int(xx + .5) + _xsize/2;
	oy = _ysize/2 - int(yy + .5);

	return YARP_OK;
}

/// out -> 256 x 256, in 152 x 256.
///
int YARPLogpolar::Logpolar2Cartesian (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
{
	using namespace _logpolarParams;
	ACE_ASSERT (in.GetWidth() == _stheta && in.GetHeight() == _srho);
	ACE_ASSERT (out.GetWidth() == _xsize && out.GetHeight() == _ysize);

	Remap ((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_img, _remapMap);
	return YARP_OK;
}

int YARPLogpolar::Logpolar2CartesianFovea (const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
{
	using namespace _logpolarParams;
	ACE_ASSERT (in.GetWidth() == _stheta && in.GetHeight() == _srho);
	ACE_ASSERT (out.GetWidth() == _xsizefovea && out.GetHeight() == _ysizefovea);

	_img.Size_X_Remap = _xsizefovea;
	_img.Size_Y_Remap = _ysizefovea;
	_img.Size_Img_Remap = _xsizefovea * _ysizefovea;
	_img.Zoom_Level = 512.0/1090.0;
		
	Remap ((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_img, _remapMapFovea);

	_img.Size_X_Remap = _xsize;
	_img.Size_Y_Remap = _ysize;
	_img.Size_Img_Remap = _xsize * _ysize;
	_img.Zoom_Level = 256.0/1090.0;

	return YARP_OK;
}

///
/// ix, iy are in [0,max]x[0,max]
int YARPLogpolar::Cartesian2Logpolar (int ix, int iy, int& orho, int& otheta)
{
	using namespace _logpolarParams;
	ix -= _xsize/2;
	iy = _ysize/2 - iy;

	orho = Get_Rho (ix, iy, &_img);
	otheta = Get_Theta(ix, iy, orho, &_img, _angShiftMap, _padMap);

	return YARP_OK;
}

int YARPLogpolar::ReconstructColor (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelBGR> & out)
{
	using namespace _logpolarParams;

	Reconstruct_Color((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), _srho, _stheta, _img.padding, _weightsMap, _img.Pix_Numb);
	return YARP_OK;
}

int YARPLogpolar::ReconstructGrays (const YARPImageOf<YarpPixelMono>& in, YARPImageOf<YarpPixelMono>& out)
{
	using namespace _logpolarParams;

	Reconstruct_Grays((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), _srho, _stheta, _img.padding, _weightsMap, _img.Pix_Numb);
	return YARP_OK;
}

int YARPLogpolar::Sawt2Uniform(const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
{
	using namespace _logpolarParams;

	sawt2Uniform((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_img, _padMap);
	return YARP_OK;
}

int YARPLogpolar::Uniform2Sawt(const YARPImageOf<YarpPixelBGR>& in, YARPImageOf<YarpPixelBGR>& out)
{
	using namespace _logpolarParams;

	uniform2Sawt((unsigned char *)out.GetRawBuffer(), (unsigned char *)in.GetRawBuffer(), &_img, _padMap);
	return YARP_OK;
}