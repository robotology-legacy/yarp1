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
/// $Id: teststereomatch.cpp,v 1.2 2004-07-30 17:41:22 babybot Exp $
/// 
/// Test stereo match by mergin together left and right channels (fovea)
/// January 04 -- by nat

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPImages.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPLogpolar.h>
#include <yarp/YARPMath.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageUtils.h>
#include <yarp/YARPColorConverter.h>

///
///
///
YARPInputPortOf<YARPGenericImage> _inLeftPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
YARPInputPortOf<YARPGenericImage> _inRightPort(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);

YARPOutputPortOf<YARPGenericImage> _outImgPort (YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

const char *DEFAULT_NAME = "/teststereomatch";

///
///
///
///
void printFrame(int c);

int main(int argc, char *argv[])
{
	using namespace _logpolarParams;
	YARPLogpolar mapper;

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
		network_i = "Net1";
	}

	if (!YARPParseParameters::parse(argc, argv, "neto", network_o))
	{
		network_o = "default";
	}

	/// images are coming from the input network.
	sprintf(buf, "%s/i:left", name.c_str());
	_inLeftPort.Register(buf, network_i.c_str());
	sprintf(buf, "%s/i:right", name.c_str());
	_inRightPort.Register(buf, network_i.c_str());
	sprintf(buf, "%s/o:img", name.c_str());
	_outImgPort.Register(buf, network_o.c_str());

	YARPImageOf<YarpPixelMono> inl;
	YARPImageOf<YarpPixelMono> inr;

	YARPImageOf<YarpPixelBGR> mergedImg;
	
	YARPImageOf<YarpPixelBGR> colLeft, fovLeft;
	YARPImageOf<YarpPixelBGR> colRight, fovRight;
	YARPImageOf<YarpPixelMono> fovLeftGray, fovRightGray;

	// check size for fovea images
	colLeft.Resize (_stheta, _srho);
	colRight.Resize (_stheta, _srho);
	fovLeft.Resize(_xsizefovea, _ysizefovea);
	fovRight.Resize(_xsizefovea, _ysizefovea);
	mergedImg.Resize(_xsizefovea, _ysizefovea);
	fovLeftGray.Resize(_xsizefovea, _ysizefovea);
	fovRightGray.Resize(_xsizefovea, _ysizefovea);
	
	int frameCounter = 0;

	while (1)
	{
		_inLeftPort.Read();
		_inRightPort.Read();

		inl.Refer (_inLeftPort.Content());
		inr.Refer (_inRightPort.Content());

		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inl, colLeft);
		mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)inr, colRight);
		mapper.Logpolar2CartesianFovea (colLeft, fovLeft);
		mapper.Logpolar2CartesianFovea (colRight, fovRight);

		YARPColorConverter::RGB2Grayscale(fovRight, fovRightGray);
		YARPColorConverter::RGB2Grayscale(fovLeft, fovLeftGray);

		YARPImageUtils::SetRed(fovLeftGray, mergedImg);
		YARPImageUtils::SetGreen(fovRightGray, mergedImg);
		
		_outImgPort.Content().Refer(mergedImg);
		_outImgPort.Write();

		printFrame(frameCounter);
		frameCounter++;
	}

	return 0;
}

void printFrame(int c)
{	
	if (c%500 == 0)
		ACE_OS::printf("Frame #:%d\n", c);
}
