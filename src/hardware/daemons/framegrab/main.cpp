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
/// $Id: main.cpp,v 1.46 2004-01-21 09:41:43 beltran Exp $
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
#include <YARPBottle.h>
#include <YARPBottleContent.h>

#if defined(__QNXEurobot__)

#	include <YARPEurobotGrabber.h>
#	define Grabber YARPEurobotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__WIN32Babybot__)

#	include <YARPBabybotGrabber.h>
#	define Grabber YARPBabybotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__QNXBabybot__)

#	include <YARPBabybotGrabber.h>
#	define Grabber YARPBabybotGrabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#elif defined(__LinuxTest__)

#	define Grabber
#	define DeclareOutport(x) YARPOutputPortOf<YARPGenericImage>##x(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)

#else

#error "pls specify a setup by defining symbol, see code above"

#endif

///
/// global params.
int		_sizex		= -1;
int		_sizey		= -1;
char	_name[512];
char	_fgdataname[512];
char	_netname[512];
bool	_client		= false;
bool	_simu		= false;
bool	_logp		= false;
int		_board_no	= 0;
bool	_fgnetdata	= false;

extern int __debug_level;


/// LATER: used to parse command line options.
int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/o:img\0", argv[0]);
	ACE_OS::sprintf (_fgdataname,"/%s/i:fgdata\0", argv[0]);
	ACE_OS::sprintf (_netname, "default\0");
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+')
		{
			ACE_OS::sprintf (_name, "/%s/o:img\0", argv[i]+1);
			ACE_OS::sprintf (_fgdataname,"/%s/i:fgdata\0", argv[i]+1);
		}
		else
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'w':
				_sizex = ACE_OS::atoi (argv[i+1]);
				ACE_ASSERT (_sizex <= 384);
				ACE_ASSERT ((_sizex % 8) == 0);
				i++;
				break;

			case 'h':
				_sizey = ACE_OS::atoi (argv[i+1]);
				ACE_ASSERT (_sizey <= 288);
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

			case 'n':
				ACE_OS::fprintf (stdout, "sending to network : %s\n", argv[i+1]);
				ACE_OS::sprintf (_netname, "%s\0", argv[i+1]);
				i++;
				break;

			case 'f':
				ACE_OS::fprintf(stdout, "grabber receiving data from network mode...\n");
				_fgnetdata = true;
				break;
			}
		}
		else
		{
			ACE_OS::fprintf (stderr, "unrecognized parameter %d:%s\n", i, argv[i]);
		}
	}

	if (_sizex == -1 && _sizey != -1)
		_sizex = _sizey;
	else
	if (_sizex != -1 && _sizey == -1)
		_sizey = _sizex;
	else
	if (_sizex == -1 && _sizey == -1)
		_sizex = _sizey = 128;

	using namespace _logpolarParams;
	if (_logp)
	{
		_sizex = _xsize;
		_sizey = _ysize;
	}

	return YARP_OK; 
}

///
///
///
int _grabber2rgb (const unsigned char *in, unsigned char *out, int szx, int szy)
{
	int * first_pix = (int *)in;
	int * ptr = first_pix;

	for (int r = 0; r < szy; r++)
	{
		ptr = first_pix + (szx * r);

		/// needed to remove the extra byte of the 32 bit representation.
		for (int c = 0; c < szx; c++)
		{
			*out++ = (unsigned char)((*ptr & 0x000000ff));
			*out++ = (unsigned char)((*ptr & 0x0000ff00) >> 8);
			*out++ = (unsigned char)((*ptr & 0x00ff0000) >> 16);

			ptr ++;
		}
	}
	
	return YARP_OK;
}

// =====================================================================================
//        Class:  FgNetDataPort
// 
//  Description:  This class extends a YARPPort in order to implement the OnRead callback function
// 				  It receives a bottle with information to adjust the images in the framegrabber
// 				  To be used to fgadjuster photon application. 
// 
//       Author:  Ing. Carlos Beltran
//      Created:  15/01/2003 10:36:00 W. Europe Standard Time
//     Revision:  21/01/2004: Removed some prints
// =====================================================================================

class FgNetDataPort : public YARPInputPortOf<YARPBottle>
{
	protected:
		YARPBottle m_bottle;
		Grabber * m_gb;
		unsigned int m_bright;
		unsigned int m_hue;
		unsigned int m_contrast;
		unsigned int m_satu;
		unsigned int m_satv;
		int m_lnotch;
		int m_ldec;
		int m_crush;

	public:
		FgNetDataPort (Grabber * gb):YARPInputPortOf<YARPBottle>(YARPInputPort::DOUBLE_BUFFERS, YARP_UDP) 
		{ 
			m_gb = gb;
			m_bottle.reset();
		}

		virtual void OnRead(void);
};

void FgNetDataPort::OnRead(void)
{
	//printf("Accesing OnRead\n");
	Read ();
	m_bottle = Content();
	m_bottle.readInt((int *)&m_bright);
	m_bottle.readInt((int *)&m_hue);
	m_bottle.readInt((int *)&m_contrast);
	m_bottle.readInt((int *)&m_satu);
	m_bottle.readInt((int *)&m_satv);
	m_bottle.readInt((int *)&m_lnotch);
	m_bottle.readInt((int *)&m_ldec);
	m_bottle.readInt((int *)&m_crush);

	//m_bottle.display();

	m_gb->setBright((unsigned int)m_bright);
	m_gb->setHue((unsigned int)m_hue);
	m_gb->setContrast((unsigned int)m_contrast);
	m_gb->setSatU((unsigned int)m_satu);
	m_gb->setSatV((unsigned int)m_satv);
	m_gb->setLNotch(m_lnotch);
	m_gb->setLDec(m_ldec);
	m_gb->setCrush(m_crush);
}

///
///
///
#if 0
#include <signal.h>

bool finished = false;

void _hh (int sig)
{
	ACE_UNUSED_ARG(sig);

	finished = true;
	ACE_OS::signal (SIGINT, _hh);
}
#endif

///
///
///
///
class mainthread : public YARPThread
{
public:
	virtual void Body (void)
	{
		if (_client)
		{
			_runAsClient ();
			return;
		}
		else
		if (_simu && !_logp)
		{
			_runAsSimulation ();
			return;
		}
		else
		if (_simu && _logp)
		{
			_runAsLogpolarSimulation ();
			return;
		}
#if !defined(__LinuxTest__)
		else
		if (_logp)
		{
			_runAsLogpolar ();
			return;
		}
		else
		{
			_runAsCartesian ();
			return;
		}
#endif
	}

	int _runAsClient(void);
	int _runAsSimulation (void);
	int _runAsLogpolarSimulation (void);
	int _runAsLogpolar (void);
	int _runAsCartesian (void);
};


int mainthread::_runAsClient (void)
{
#if !defined(__LinuxTest__)
	YARPImageOf<YarpPixelMono> img;
#else
	YARPImageOf<YarpPixelBGR> img;
#endif
	YARPInputPortOf<YARPGenericImage> inport(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	inport.Register (_name, _netname);
	int frame_no = 0;

	char savename[512];
	memset (savename, 0, 512);

	while (!IsTerminated())
	{
		inport.Read ();
		img.Refer (inport.Content());

		ACE_OS::printf (">>> got a frame\n");

		frame_no++;

		ACE_OS::sprintf (savename, "./grab_test%04d.ppm\0", frame_no);
		YARPImageFile::Write (savename, img);

///		if (frame_no > 10)
///		{
///			finished = true;
///		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


int mainthread::_runAsSimulation (void)
{
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_sizex, _sizey);
	img.Zero ();

	DeclareOutport(outport);

	outport.Register (_name, _netname);

	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _sizex, _sizey);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
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
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			// ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}


int mainthread::_runAsLogpolarSimulation (void)
{
	using namespace _logpolarParams;

	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelMono> lp;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);
	ACE_ASSERT (_xsize == _sizex && _ysize == _sizey);

	img.Resize (_xsize, _ysize);
	lp.Resize (_stheta, _srho);

	DeclareOutport(out);

	out.Register (_name, _netname);
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up simulation of a grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", _xsize, _ysize);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	char * path = ACE_OS::getenv ("YARP_ROOT");
	char filename[512];
#ifdef __WIN32__
	ACE_OS::sprintf (filename, "%s\\conf\\test_grabber.ppm\0", path);
#else
	ACE_OS::sprintf (filename, "%s/conf/test_grabber.ppm\0", path);
#endif
	YARPImageFile::Read (filename, img);

	while (!IsTerminated())
	{
		YARPTime::DelayInSeconds (0.04);
		
		/// blink
		if ((frame_no % 2) == 0)
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = -1;
		else
			*(img.GetRawBuffer() + 128 * 256 * 3 + 128 * 3) = 0;

		sampler.Cartesian2Logpolar (img, lp);

		/// sends the buffer.
		out.Content().Refer (lp);
		out.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			// ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

#if !defined(__LinuxTest__)
int mainthread::_runAsLogpolar (void)
{
	using namespace _logpolarParams;

	Grabber grabber;

	YARPImageOf<YarpPixelBGR> img;
	YARPImageOf<YarpPixelMono> lp;
	
	YARPLogpolarSampler sampler;

	ACE_ASSERT (_xsize == _ysize);
	ACE_ASSERT (_xsize == _sizex && _ysize == _sizey);

	img.Resize (_xsize, _ysize);
	lp.Resize (_stheta, _srho);

	DeclareOutport(out);
	out.Register (_name, _netname);

	/// params to be passed from the command line.
	grabber.initialize (_board_no, _xsize);

	//Activate port to receive image adjustment data
	FgNetDataPort  * m_fg_net_data;
	if (_fgnetdata)
	{
		m_fg_net_data = new FgNetDataPort(&grabber);
		m_fg_net_data->Register (_fgdataname,_netname);
	}

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is logpolar\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer (&buffer);
	
		/// fills the actual image buffer.
		memcpy((unsigned char *)img.GetRawBuffer(), buffer, _xsize * _xsize * 3);
		
		grabber.releaseBuffer ();

		sampler.Cartesian2Logpolar (img, lp);

		/// sends the buffer.
		out.Content().Refer (lp);
		out.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			//ACE_OS::fprintf (stdout, "frame number %d acquired\n", frame_no);
			start = cur;
		}
	}

	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; //can this be done better? (closind the port?)
	}

	grabber.uninitialize ();
	
	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}

int mainthread::_runAsCartesian (void)
{
	Grabber grabber;
	YARPImageOf<YarpPixelBGR> img;
	img.Resize (_sizex, _sizey);

	DeclareOutport(outport);

	outport.Register (_name, _netname);

	/// params to be passed from the command line.
	grabber.initialize (_board_no, _sizex, _sizey);
	
	FgNetDataPort  * m_fg_net_data;
	if (_fgnetdata)
	{
		m_fg_net_data = new FgNetDataPort(&grabber);
		m_fg_net_data->Register (_fgdataname,_netname);
	}

	int w = -1, h = -1;
	grabber.getWidth (&w);
	grabber.getHeight (&h);

	unsigned char *buffer = NULL;
	int frame_no = 0;

	ACE_OS::fprintf (stdout, "starting up grabber...\n");
	ACE_OS::fprintf (stdout, "grabber is cartesian\n");
	ACE_OS::fprintf (stdout, "acq size: w=%d h=%d\n", w, h);

	double start = YARPTime::GetTimeAsSeconds ();
	double cur = start;

	while (!IsTerminated())
	{
		grabber.waitOnNewFrame ();
		grabber.acquireBuffer(&buffer);

		memcpy((unsigned char *)img.GetRawBuffer(), buffer, _sizex * _sizey * 3);

		grabber.releaseBuffer ();

		/// sends the buffer.
		outport.Content().Refer (img);
		outport.Write();

		frame_no++;
		if ((frame_no % 250) == 0)
		{
			cur = YARPTime::GetTimeAsSeconds ();
			ACE_OS::fprintf (stdout, "average frame time: %f frame #%d acquired\r", (cur-start)/250, frame_no);
			// ACE_OS::fprintf (stdout, "frame number %d acquired\r", frame_no);
			start = cur;
		}
	}

	//destroy fg_net_data port
	if (_fgnetdata)
	{
		if (m_fg_net_data != NULL)
			delete m_fg_net_data; //can this be done better? (closind the port?)
	}

	grabber.uninitialize ();

	ACE_OS::fprintf (stdout, "returning smoothly\n");
	return YARP_OK;
}
#endif

///
///
///
int main (int argc, char *argv[])
{
	///__debug_level = 80;

	YARPScheduler::setHighResScheduling ();

	ParseParams (argc, argv);

///	ACE_OS::signal (SIGINT, _hh);

	mainthread _thread;
	_thread.Begin();

	char c = 0;

	do
	{
		cout << "Type q+return to quit" << endl;
		cin >> c;
	}
	while (c != 'q');

	_thread.End(-1);

	return YARP_OK;
}



