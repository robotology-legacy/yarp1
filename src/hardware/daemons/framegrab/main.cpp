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
/// $Id: main.cpp,v 1.6 2003-06-05 17:48:13 babybot Exp $
///
///



#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>

#include <YARPBabybotGrabber.h>

#include <YARPImages.h>

const int size = 128;
char _name[512];
bool _client = false;

/// LATER: used to parse command line options.
int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/o:img\0", argv[0]);
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+')
		{
			memcpy (_name, &argv[i][1], strlen (&argv[i][1]));
		}
		else
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'w':
				i++;
				break;

			case 'h':
				i++;
				break;

			case 't':
				ACE_OS::fprintf (stdout, "grabber acting as a receiver client...\n");
				ACE_OS::sprintf (_name, "%s\0", argv[i+1]);
				_client = true;
				i++;
				break;
			}
		}
		else
		{
			ACE_OS::fprintf (stderr, "unrecognized parameter %d:%s\n", i, argv[i]);
		}
	}

	return YARP_OK; 
}

///
///
///
int _grabber2rgb (const unsigned char *in, unsigned char *out, int sz)
{
	int * first_pix = (int *)in;
	int * ptr = first_pix;

	///for (int r = 2 * sz - 1; r >= 0; r--, r--)
	for (int r = 0; r < 2 * sz; r++, r++)
	{
		ptr = first_pix + (sz * r);

		/// needed to remove the extra byte of the 32 bit representation.
		for (int c = 0; c < sz; c++)
		{
			*out++ = unsigned char((*ptr & 0x00ff0000) >> 16);
			*out++ = unsigned char((*ptr & 0x0000ff00) >> 8);
			*out++ = unsigned char((*ptr & 0x000000ff));

			ptr ++;
		}
	}
	
	return YARP_OK;
}

int _runAsClient (void)
{
	YARPImageOf<YarpPixelBGR> img;
	///img.Resize (size, size);

	YARPInputPortOf<YARPGenericImage> inport;
	bool finished = false;

	inport.Register (_name);
	int frame_no = 0;

	char savename[512];
	memset (savename, 0, 512);

	while (!finished)
	{
		ACE_OS::printf (">>> before read\n");

		inport.Read ();
		img.Refer (inport.Content());

		ACE_OS::printf (">>> got a frame\n");

		frame_no++;

		ACE_OS::sprintf (savename, "./grab_test%d.ppm\0", frame_no);
		YARPImageFile::Write (savename, img);

		if (frame_no > 10)
		{
			finished = true;
		}
	}

	return YARP_OK;
}

///
///
///
int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling ();

	ParseParams (argc, argv);

	if (_client)
	{
		return _runAsClient();
	}

	YARPBabybotGrabber grabber;
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (size, size);

	YARPOutputPortOf<YARPGenericImage> outport;
	bool finished = false;

	outport.Register (_name);

	/// params to be passed from the command line.
	grabber.initialize (0, size);

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stderr, "starting up grabber...\n");
	ACE_OS::fprintf (stderr, "acq size: w=%d h=%d\n", w, h);

	if (w != size || h != 2 * size)
	{
		ACE_OS::fprintf (stderr, "pls, specify a different size, application will now exit\n");
		finished = true;
	}

	while (!finished)
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer (&buffer);
	
		/// fills the actual image buffer.
		_grabber2rgb (buffer, (unsigned char *)img.GetRawBuffer(), size);
		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
			ACE_OS::fprintf (stderr, "frame number %d acquired\n", frame_no);

		/// sends the buffer.

		grabber.releaseBuffer ();
	}

	grabber.uninitialize ();

	return YARP_OK;
}



