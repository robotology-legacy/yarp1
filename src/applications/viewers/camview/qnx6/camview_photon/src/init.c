/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

/* YARP/ACE headers */

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPPort.h>
#include <YARPImages.h>
#include <YARPDIBConverter.h>
#include <YARPLogpolar.h>
#include <iostream>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>
#include <YARPImageFile.h>
#include <YARPControlBoardNetworkData.h>

extern PhImage_t * phimage;
int W = 128;
int H = 128;
int WH = 128;
int _board_no = 0; 
int _video_mo = 0;
char _name[512];
bool _client = false;
bool _simu = false;
bool _logp = false;
bool _fov = false;

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */

class ReadingThread: public YARPThread
{
public:
    
    YARPGenericImage img;
    YARPGenericImage m_flipped;
	YARPImageOf<YarpPixelBGR> m_remapped;
	YARPImageOf<YarpPixelBGR> m_colored;
	//YARPDIBConverter m_converter;
	YARPLogpolar m_mapper;
	int counter;

	virtual void Body()
	{
		using namespace _logpolarParams;

		int ct = 0;
		YARPInputPortOf<YARPGenericImage> inport (YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
		inport.Register ("/images:i");
		phimage->bpl = W * 3;

		while (1)
		{
			inport.Read ();	  
			img.Refer (inport.Content());

			if (!_logp)
			{
			   //image->buffer = (unsigned char *)img.GetRawBuffer();
			   //BlitBuffer(win,image);
				phimage->image = (char *)img.GetRawBuffer();
			   
			   counter++;
			   
			 }else //Log polar case
			 if (_logp && !_fov)
			 {
				 /*
		 		if (img.GetWidth() != _stheta || img.GetHeight() != _srho - _sfovea)
				{
					/// falls back to cartesian mode.
					_logp = false;
					continue;
				}*/
				
				//YARPImageFile::Write("image0.PGM",img);

				if (m_remapped.GetWidth() != 256 || m_remapped.GetHeight() != 256)
				{
					m_remapped.Resize (256, 256);
				}

				if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho )
				{
					m_colored.Resize (_stheta, _srho);
				}

				if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
				{
					m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
				}

				m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)img, m_colored);
				m_mapper.Logpolar2Cartesian (m_colored, m_remapped);
				//YARPSimpleOperation::Flip (m_remapped, m_flipped);
				
				//YARPImageFile::Write("remapped0.PPM",m_remapped);
				//exit(0);
				
				 ///image->buffer = (unsigned char *)m_remapped.GetRawBuffer();
				phimage->image = (char *)m_remapped.GetRawBuffer();

			 }else
			 if (_logp && _fov)
			 {
				/*
				if (img.GetWidth() != _stheta || img.GetHeight() != _srho)
				{
					/// falls back to cartesian mode.
					_logp = false;
					continue;
				}
				*/

				if (m_remapped.GetWidth() != 128 || m_remapped.GetHeight() != 128)
				{
					m_remapped.Resize (128, 128);
				}

				if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho)
				{
					m_colored.Resize (_stheta, _srho);
				}

				if (m_flipped.GetWidth() != m_remapped.GetWidth() || m_flipped.GetHeight() != m_remapped.GetHeight())
				{
					m_flipped.Resize (m_remapped.GetWidth(), m_remapped.GetHeight(), m_remapped.GetID());
				}

				m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)img, m_colored);
				m_mapper.Logpolar2CartesianFovea (m_colored, m_remapped);

				m_remapped.Resize (256, 256);

				//////image->buffer = (unsigned char *)m_remapped.GetRawBuffer();
				phimage->image = (char *)m_remapped.GetRawBuffer();

				///////BlitBuffer(win,image);
				
			 }
		}
	}
};

ReadingThread t1;

int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/i:img\0", argv[0]);
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
				W = ACE_OS::atoi (argv[i+1]);
				i++;
				break;

			case 'h':
				H = ACE_OS::atoi (argv[i+1]);
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

			case 'v':
				_video_mo = ACE_OS::atoi(argv[i+1]);
				ACE_OS::fprintf (stdout, "video mode %d...\n",_video_mo);
				break;

			case 'l':
				ACE_OS::fprintf (stdout, "logpolar mode\n");
				_logp = true;
				break;
			case 'f':
				ACE_OS::fprintf (stdout, "Fovea mode mode\n");
				_fov = true;
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

int
init( int argc, char *argv[] )

	{

	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	t1.Begin();

	//t1.End(0);
	ParseParams (argc, argv);

	return( Pt_CONTINUE );

	}

