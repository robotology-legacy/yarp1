#ifndef __READINGTHREADH__
#define __READINGTHREADH__

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
extern int W;
extern int H;
extern int WH;
extern int net_img_h;
extern int net_img_w;
extern int _board_no; 
extern int _video_mo;
extern char _name[512];
extern bool _client;
extern bool _simu;
extern bool _logp;
extern bool _fov;
extern bool freeze;
extern bool m_exit;
extern char _name[];

class ReadingThread: public YARPThread
{
	public:
		YARPGenericImage img;
		YARPImageOf<YarpPixelBGR> m_remapped;
		YARPImageOf<YarpPixelBGR> m_colored;
		YARPLogpolar m_mapper;
		int counter;

		virtual void Body()
		{
			using namespace _logpolarParams;

			int ct = 0;
			YARPInputPortOf<YARPGenericImage> inport (YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
			//inport.Register ("/images:i");
			inport.Register(_name);

			if (_logp && !_fov)
			{
				net_img_w = 256;
				net_img_h = 256;
			}

			while (!IsTerminated())
			{
				if (m_exit)
					return;
				if (!freeze)
				{
					inport.Read ();	  
					img.Refer (inport.Content());
				}
				if (!_logp) //**********************NO LOG POLAR*****************************
				{
					if ( phimage != NULL)
						phimage->image = (char *)img.GetRawBuffer();
					net_img_w = img.GetWidth();
					net_img_h = img.GetHeight();  
					counter++;
				}
				else	if (_logp && !_fov) //*************LOG POLAR*********************************
				{
					net_img_w = 256;
					net_img_h = 256;
					/*
								if (img.GetWidth() != _stheta || img.GetHeight() != _srho - _sfovea)
								{
					/// falls back to cartesian mode.
					_logp = false;
					continue;
					}*/

					if (m_remapped.GetWidth() != 256 || m_remapped.GetHeight() != 256)
					{
						m_remapped.Resize (256, 256);
					}
					if (m_colored.GetWidth() != _stheta || m_colored.GetHeight() != _srho )
					{
						m_colored.Resize (_stheta, _srho);
					}

					m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)img, m_colored);
					m_mapper.Logpolar2Cartesian (m_colored, m_remapped);
					if ( phimage != NULL)
						phimage->image = (char *)m_remapped.GetRawBuffer();

				}else if (_logp && _fov)
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

					m_mapper.ReconstructColor ((const YARPImageOf<YarpPixelMono>&)img, m_colored);
					m_mapper.Logpolar2CartesianFovea (m_colored, m_remapped);

					m_remapped.Resize (256, 256);
					if ( phimage != NULL)
						phimage->image = (char *)m_remapped.GetRawBuffer();
				}
			}
		}
};
#endif
