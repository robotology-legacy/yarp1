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
///                    #original paulfitz, changed pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: attn_tracker.cpp,v 1.3 2003-10-24 14:52:48 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <YARPImages.h>

#include <iostream>
#include <math.h>

#include <YARPTime.h>
#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPImageDraw.h>
#include <YARPLogpolar.h>
#include <YARPMath.h>

#include <YARPParseParameters.h>
#include <YARPVectorPortContent.h>

#include "ImgTrack.h"

///
///
///
YARPInputPortOf<YARPGenericImage> in_img;
YARPOutputPortOf<YARPGenericImage> out_img;
YARPOutputPortOf<YVector> out_point;

const char *DEFAULT_NAME = "/tracker";

///
///
///
///
int main(int argc, char *argv[])
{
	using namespace _logpolarParams;

	YARPComplexTrackerTool tracker;

	YARPString name;
	YARPString network;
	char buf[256];

	if (!YARPParseParameters::parse(argc, argv, "name", name))
	{
		name = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "net", network))
	{
		network = "default";
	}

	sprintf(buf, "%s/i:img", name.c_str());
	in_img.Register(buf, network.c_str());
	sprintf(buf, "%s/o:img", name.c_str());
	out_img.Register(buf, network.c_str());

	sprintf(buf, "%s/o:vect", name.c_str());
	out_point.Register(buf, network.c_str());

	///
	///
	///
	YARPImageOf<YarpPixelMono> in;
	YARPImageOf<YarpPixelBGR> out;
	YARPImageOf<YarpPixelBGR> colored;
	YARPImageOf<YarpPixelBGR> remapped;

	colored.Resize (_stheta, _srho);
	remapped.Resize (128, 128);

	YARPLogpolar mapper;
	YVector v(2);
	v = 0;

	while(1)
    {
		in_img.Read();

		in.Refer (in_img.Content());
		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)in, colored);
		mapper.Logpolar2CartesianFovea (colored, remapped);

		ACE_ASSERT (in.GetWidth() == _stheta && in.GetHeight() == _srho);
		ACE_ASSERT (remapped.GetWidth() == 128 && remapped.GetHeight() == 128);

		out_img.Content().SetID (YARP_PIXEL_BGR);

		SatisfySize (remapped, out_img.Content());
		out.Refer (out_img.Content());
		tracker.apply (remapped, out);
		out_img.Write ();

		/// get the target.
		int x = 64, y = 64;
		tracker.getTarget (x, y);
		v(1) = x;
		v(2) = y;
		out_point.Content() = v;
		out_point.Write();
    }

	return 0;
}
