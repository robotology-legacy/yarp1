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
/// $Id: main.cpp,v 1.24 2003-07-15 14:20:39 beltran Exp $
///
///



#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>

#include <YARPImages.h>
#include <YARPLogpolar.h>


#if defined(__QNXEurobot__)

#	include <YARPEurobotGrabber.h>
#	define Grabber YARPEurobotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage> ##x

#elif defined(__WIN32Babybot__)

#	include <YARPBabybotGrabber.h>
#	define Grabber YARPBabybotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage> ##x

#elif defined(__QNXBabybot__)

#	include <YARPBabybotGrabber.h>
#	define Grabber YARPBabybotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage> ##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)

#else

#error "pls specify a setup by defining symbol, see code above"

#endif

///
/// global params.
int _size = 128;
char _name[512];
bool _client = false;
bool _simu = false;
bool _logp = false;
int _board_no = 0;


extern int __debug_level;


/// LATER: used to parse command line options.
int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/o:img\0", argv[0]);
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+')
		{
			ACE_OS::sprintf (_name, "/%s/o:img\0", argv[i]+1);
		}
		else
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'w':
				_size = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 'h':
				_size = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 'b':
				_board_no = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 't':
				ACE_OS::fprintf (stdout, "grabber acting as a receiver client...\n");
				ACE_OS::sprintf (_name, "%s\0", argv[i+1]);
				i++;
				_client = true;
				_simu = false;
				break;

			case 's':
				ACE_OS::fprintf (stdout, "simulating a grabber...\n");
				_simu = true;
				_client = false;
				break;

			case 'l':
				ACE_OS::fprintf (stdout, "logpolar mode\n");
				_logp = true;
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

	for (int r = 0; r < sz; r++)
	{
		ptr = first_pix + (sz * r);

		/// needed to remove the extra byte of the 32 bit representation.
		for (int c = 0; c < sz; c++)
		{
			*out++ = (unsigned char)((*ptr & 0x000000ff));
			*out++ = (unsigned char)((*ptr & 0x0000ff00) >> 8);
			*out++ = (unsigned char)((*ptr & 0x00ff0000) >> 16);

			ptr ++;
		}
	}
	
	return YARP_OK;
}


#include <signal.h>

bool finished = false;

void _hh (int sig)
{
	finished = true;
	ACE_OS::signal (SIGINT, _hh);
}


int _runAsClient (void)
{
	YARPImageOf<YarpPixelBGR> img;

	YARPInputPortOf<YARPGenericImage> inport;

	inport.Register (_name);
	int frame_no = 0;

	char savename[512];
	memset (savename, 0, 512);

	while (!finished)
	{
		inport.Read ();
		img.Refer (inport.Content());

		ACE_OS::printf (">>> got a frame\n");

		frame_no++;

		ACE_OS::sprintf (savename, "./grab_test%04d.ppm\0", frame_no);
		YARPImageFile::Write (savename, img);

		if (frame_no > 100)
		{
			finished = true;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


int _runAsSimulation (void)
{
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_size, _size);
	img.Zero ();

	DeclareOutport(outport);

	///bool finished = false;

	outport.Register (_name);

	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _size, _size);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!finished)
	{
		YARPTime::DelayInSeconds (0.04);
	
		img.Zero ();
		*(img.GetRawBuffer() + (frame_no % img.GetAllocatedDataSize())) = -1;

		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %lf\n", (cur-start)/250);
			ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


int _runAsLogpolarSimulation (void)
{
	using namespace _logpolarParams;

	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelBGR> fovea;
	YARPImageOf<YarpPixelMono> periphery;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);

	img.Resize (_xsize, _ysize);
	fovea.Resize (128, 128);
	periphery.Resize (_stheta, _srho - _sfovea);

	DeclareOutport(out_fovea);
	DeclareOutport(out_periphery);

	out_fovea.Register (_name);
	char name_p[512];
	sprintf (name_p, "%sp\0", _name);
	out_periphery.Register (name_p);

	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _xsize, _ysize);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;
	///bool finished = false;

	char * path = ACE_OS::getenv ("YARP_ROOT");
	char filename[512];
#ifdef __WIN32__
	ACE_OS::sprintf (filename, "%s\\conf\\test_grabber.ppm\0", path);
#else
	ACE_OS::sprintf (filename, "%s/conf/test_grabber.ppm\0", path);
#endif
	YARPImageFile::Read (filename, img);

	while (!finished)
	{
		YARPTime::DelayInSeconds (0.04);
		
		/// blink
		if ((frame_no % 2) == 0)
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = -1;
		else
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = 0;

		sampler.Cartesian2Logpolar (img, fovea, periphery);

		/// sends the buffer.
		out_fovea.Content().Refer (fovea);
		out_fovea.Write();

		out_periphery.Content().Refer (periphery);
		out_periphery.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %lf\n", (cur-start)/250);
			ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


int _runAsLogpolar (void)
{
	using namespace _logpolarParams;

	Grabber grabber;
	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelBGR> fovea;
	YARPImageOf<YarpPixelMono> periphery;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);

	img.Resize (_xsize, _ysize);
	fovea.Resize (128, 128);
	periphery.Resize (_stheta, _srho - _sfovea);

	DeclareOutport(out_fovea);
	DeclareOutport(out_periphery);
	///bool finished = false;

	out_fovea.Register (_name);

	char name_p[512];
	sprintf (name_p, "%sp\0", _name);
	out_periphery.Register (name_p);

	/// params to be passed from the command line.
	grabber.initialize (_board_no, _xsize);

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

#if 0
	if (w != _xsize || h != 2 * _xsize) ///2 * _size)
	{
		ACE_OS::fprintf (stderr, "pls, specify a different size, application will now exit\n");
		finished = true;
	}
#endif

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!finished)
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer (&buffer);
	
		/// fills the actual image buffer.
#ifdef __QNX6__
		memcpy((unsigned char *)img.GetRawBuffer(),buffer, _xsize*_xsize*3);
#else
		/// fills the actual image buffer.
		_grabber2rgb (buffer, (unsigned char *)img.GetRawBuffer(), _xsize);
#endif
		

		grabber.releaseBuffer ();

		sampler.Cartesian2Logpolar (img, fovea, periphery);

		/// sends the buffer.
		out_fovea.Content().Refer (fovea);
		out_fovea.Write();

		out_periphery.Content().Refer (periphery);
		out_periphery.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %lf\n", (cur-start)/250);
			ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	grabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

int _runAsCartesian (void)
{
	Grabber grabber;
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_size, _size);

	DeclareOutport(outport);

	////bool finished = false;

	outport.Register (_name);

	/// params to be passed from the command line.
	grabber.initialize (_board_no, _size);

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is cartesian\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

	//if (w != _size || h != 2 * _size) ///2 * _size)
	//{
	//		ACE_OS::fprintf (stderr, "pls, specify a different size, application will now exit\n");
	//	finished = true;
	//}

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!finished)
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer(&buffer);
#ifdef __QNX6__
		memcpy((unsigned char *)img.GetRawBuffer(),buffer, _size*_size*3);
#else
		/// fills the actual image buffer.
		_grabber2rgb (buffer, (unsigned char *)img.GetRawBuffer(), _size);
#endif

		/// sends the buffer.
		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %lf\n", (cur-start)/250);
			ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}

		grabber.releaseBuffer ();
	}

	grabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

///
///
///
int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling ();

	ParseParams (argc, argv);

	ACE_OS::signal (SIGINT, _hh);

	if (_client)
	{
		return _runAsClient ();
	}
	else
	if (_simu && !_logp)
	{
		return _runAsSimulation ();
	}
	else
	if (_simu && _logp)
	{
		return _runAsLogpolarSimulation ();
	}
	else
	if (_logp)
	{
		return _runAsLogpolar ();
	}
	else
	{
		return _runAsCartesian ();
	}

	return YARP_OK;
}



