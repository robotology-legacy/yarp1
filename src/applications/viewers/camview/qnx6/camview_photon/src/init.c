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
#include "ReadingThread.h"

int W = 128;
int H = 128;
int WH = 128;
int net_img_h = 128;
int net_img_w = 128;
int _board_no = 0; 
int _video_mo = 0;
char _name[512];
bool _client = false;
bool _simu = false;
bool _logp = false;
bool _fov = false;
ReadingThread t1;
bool freeze = false;
bool m_exit = false;
PhImage_t * phimage;
PhImage_t * phimage_vis;

/* Application Options string */
const char ApOptions[] =
AB_OPTIONS ""; /* Add your options in the "" */

int ParseParams (int argc, char *argv[]) 
{
	ACE_OS::sprintf (_name, "/%s/i:img\0", argv[0]);
	int i;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '+')
		{
			ACE_OS::sprintf (_name, "/%s/i:img\0", argv[i]+1);
		}
		else if (argv[i][0] == '-')
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
					net_img_h = 256;
					net_img_w = 256;
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
	ParseParams (argc, argv);

	t1.Begin();

	_Ap_.Ap_winsize.y = H+(H*0.2)+50; 
	_Ap_.Ap_winsize.x = W+20;

	return( Pt_CONTINUE );
}



