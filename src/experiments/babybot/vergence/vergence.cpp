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
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: vergence.cpp,v 1.2 2003-11-21 13:59:19 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPImages.h>
#include <YARPImagePortContent.h>

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
#include <YARPDisparity.h>

///
///
///
YARPInputPortOf<YARPGenericImage> in_left;
YARPInputPortOf<YARPGenericImage> in_right;

YARPOutputPortOf<YARPGenericImage> out_img (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

const char *DEFAULT_NAME = "/vergence";

///
///
///
///
int main(int argc, char *argv[])
{
	using namespace _logpolarParams;

	YARPString name;
	YARPString network_i;
	YARPString network_o;
	char buf[256];

	if (!YARPParseParameters::parse(argc, argv, "name", name))
	{
		name = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "neti", network_i))
	{
		network_i = "default";
	}

	if (!YARPParseParameters::parse(argc, argv, "neto", network_o))
	{
		network_o = "default";
	}

	/// images are coming from the input network.
	sprintf(buf, "%s/i:left", name.c_str());
	in_left.Register(buf, network_i.c_str());
	sprintf(buf, "%s/i:right", name.c_str());
	in_right.Register(buf, network_i.c_str());

	sprintf(buf, "%s/o:histo", name.c_str());
	out_img.Register(buf, network_i.c_str());

	YARPImageOf<YarpPixelMono> inl;
	YARPImageOf<YarpPixelMono> inr;

	YARPImageOf<YarpPixelMono> out;
	
	YARPImageOf<YarpPixelBGR> col_left;
	YARPImageOf<YarpPixelBGR> col_right;

	YARPImageOf<YarpPixelBGR> sub_left;
	YARPImageOf<YarpPixelBGR> sub_right;

	col_left.Resize (_stheta, _srho);
	col_right.Resize (_stheta, _srho);
	sub_left.Resize (_stheta/4, _srho/4);
	sub_right.Resize (_stheta/4, _srho/4);

	YARPLogpolar mapper;
	YARPDisparityTool disparity;
	disparity._imgL = disparity.lpInfo (_xsize, _ysize, _srho, _stheta, _sfovea, 1090, 1.00, YarpImageAlign);
	disparity._imgS = disparity.lpInfo (_xsize, _ysize, _srho, _stheta, _sfovea, 1090, 4.00, YarpImageAlign);
	disparity.loadShiftTable (&disparity._imgS);
	disparity.loadDSTable (&disparity._imgL);

	while (1)
	{
		in_left.Read();
		in_right.Read();

		inl.Refer (in_left.Content());
		inr.Refer (in_right.Content());

		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inl, col_left);
		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inr, col_right);
		ACE_ASSERT (inl.GetWidth() == _stheta && inl.GetHeight() == _srho);
		ACE_ASSERT (inr.GetWidth() == _stheta && inr.GetHeight() == _srho);

		/// subsample
		disparity.downSample (col_left, sub_left);
		disparity.downSample (col_right, sub_right);
		
		int disparityval = disparity.computeDisparity (sub_left, sub_right);

		out_img.Content().SetID (YARP_PIXEL_MONO);
		out_img.Content().Resize (128, 64);
		out.Refer (out_img.Content());

		disparity.makeHistogram (out);
		out_img.Write();
	}

	return 0;
}
