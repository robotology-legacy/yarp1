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
/// $Id: seqgrabber.cpp,v 1.6 2005-02-25 11:05:40 babybot Exp $
/// 

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPImages.h>
#include <yarp/YARPImagePortContent.h>

#include <iostream>
#include <math.h>

#include <yarp/YARPLogpolar.h>
#include <yarp/YARPMath.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPImageFile.h>
#include <yarp/YARPImageUtils.h>
#include <yarp/YARPTime.h>

///
///
///
YARPInputPortOf<YARPGenericImage> _image1Port(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
YARPInputPortOf<YARPGenericImage> _image2Port(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);

YARPInputPortOf<YARPGenericImage> _image3Port(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
YARPInputPortOf<YARPGenericImage> _image4Port(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
YARPInputPortOf<YARPGenericImage> _image5Port(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);

const char *DEFAULT_NAME = "seqgrabber";
const char *DEFAULT_FOLDER = "C:/temp/";

const int DEFAULT_LENGTH = 30;	// seconds
const double timeFrame = 0.04;	// seconds

int each = 10;		// save each 10
///
///
///
///
void printFrame(int c, int &time1);

int main(int argc, char *argv[])
{
	using namespace _logpolarParams;
	YARPLogpolar mapper;

	YARPString name;
	YARPString network_i;
	YARPString network_o;
	YARPString outdir;
	char buf[256];
	bool stereo = false;
	bool cinque = false;
	bool stop = false;

	int length;

	if (!YARPParseParameters::parse(argc, argv, "-name", name))
	{
		name = DEFAULT_NAME;
	}

	if (!YARPParseParameters::parse(argc, argv, "-neti", network_i))
	{
		network_i = "Net1";
	}

	if (!YARPParseParameters::parse(argc, argv, "-neto", network_o))
	{
		network_o = "default";
	}

	if (!YARPParseParameters::parse(argc, argv, "-dir", outdir))
	{
		outdir = YARPString(DEFAULT_FOLDER);
	}

	if (!YARPParseParameters::parse(argc, argv, "-length", &length))
	{
		length = DEFAULT_LENGTH;
		
	}

	if (YARPParseParameters::parse(argc, argv, "-stereo"))
	{
		stereo = true;
	}

	if (YARPParseParameters::parse(argc, argv, "-cinque"))
	{
		stereo = false;
		cinque = true;
	}

	if (YARPParseParameters::parse(argc, argv, "-stop"))
	{
		stop = true;
		each = 1;
	}

	/// images are coming from the input network.
	sprintf(buf, "%s/i:left", name.c_str());
	ACE_OS::printf("%s\n", name.c_str());
	_image1Port.Register(buf, network_i.c_str());

	sprintf(buf, "%s/i:right", name.c_str());
	ACE_OS::printf("%s\n", name.c_str());
	_image2Port.Register(buf, network_i.c_str());
		
	sprintf(buf, "%s/i:3", name.c_str());
	ACE_OS::printf("%s\n", name.c_str());
	_image3Port.Register(buf, network_i.c_str());

	sprintf(buf, "%s/i:4", name.c_str());
	ACE_OS::printf("%s\n", name.c_str());
	_image4Port.Register(buf, network_i.c_str());

	sprintf(buf, "%s/i:5", name.c_str());
	ACE_OS::printf("%s\n", name.c_str());
	_image5Port.Register(buf, network_i.c_str());

	int frameCounter = 0;
	int nFrame = (int) length/timeFrame;

	char tmpName[255];

	if (!stereo)
		ACE_OS::printf("Going to save %d frames (estimated time %d) on %s\n", nFrame, length, outdir.c_str());
	else
		ACE_OS::printf("Going to save %d frames (left and right) (estimated time %d) on %s\n", nFrame, length, outdir.c_str());

	
	ACE_OS::printf("Please connect me...\n");

	int time1;
	float deltaT = 0;
	
	time1 = YARPTime::GetTimeAsSeconds ();
	while (frameCounter < nFrame)
	{		
		if (stop) {
			ACE_OS::printf("Press any key and return to continue\n");
			getchar();
		} else
			printFrame(frameCounter, time1);

		_image1Port.Read();
		if (stereo)
			_image2Port.Read();
		
		if (frameCounter%each == 0)
		{
			if (stereo)
			{
				sprintf(tmpName, "%s%sL%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
				YARPImageFile::Write(tmpName, _image1Port.Content());
			
				sprintf(tmpName, "%s%sR%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
				YARPImageFile::Write(tmpName, _image2Port.Content());
			}
			else if(cinque)
			{
				sprintf(tmpName, "%s%s1_%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
				YARPImageFile::Write(tmpName, _image1Port.Content());

				if(_image2Port.Read(0))
				{
					sprintf(tmpName, "%s%s2_%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
					YARPImageFile::Write(tmpName, _image2Port.Content());
				}

				if(_image3Port.Read(0))
				{
					sprintf(tmpName, "%s%s3_%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
					YARPImageFile::Write(tmpName, _image3Port.Content());
				}

				if(_image4Port.Read(0))
				{
					sprintf(tmpName, "%s%s4_%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
					YARPImageFile::Write(tmpName, _image4Port.Content());
				}

				if(_image5Port.Read(0))
				{
					sprintf(tmpName, "%s%s5_%05d.ppm", outdir.c_str(), name.c_str(), frameCounter);
					YARPImageFile::Write(tmpName, _image5Port.Content());
				}
			}
			else
			{
				sprintf(tmpName, "%s%s%d.ppm", outdir.c_str(), name.c_str(), frameCounter);
				YARPImageFile::Write(tmpName, _image1Port.Content());
			}
		}

		frameCounter++;
	}

	return 0;
}

void printFrame(int c, int &time1)
{	
	if (c%100 == 0)
	{
		ACE_OS::printf("Frame #:%d\n", c);
		int time2 = YARPTime::GetTimeAsSeconds ();
		float deltaT = (time2 - time1)/(float) (100);
		ACE_OS::printf("Estimated time frame %lf\n", deltaT);
		time1 = time2;
	}
}
