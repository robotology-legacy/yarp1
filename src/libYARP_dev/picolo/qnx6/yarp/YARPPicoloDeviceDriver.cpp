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
/// $Id: YARPPicoloDeviceDriver.cpp,v 1.1 2004-07-13 13:21:10 babybot Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPPicoloDeviceDriver.h"
#include <YARPFrameGrabberUtils.h>

typedef unsigned long UINT32;

class PicoloResources
{
	public:
		PicoloResources (void) : _bmutex(1), _new_frame(0)
		{
			_nRequestedSize = 0;
			_nWidth = 0;
			_nHeight = 0;
			_nImageSize = 0;

			_rawBuffer = NULL;
			_canpost = true;
		}

		~PicoloResources () { _uninitialize (); }

		enum { _num_buffers = 3 };

		YARPSemaphore _bmutex;
		YARPSemaphore _new_frame;

		// Img size are determined partially by the HW.
		UINT32 _nRequestedSize;
		UINT32 _nWidth;
		UINT32 _nHeight;
		UINT32 _nImageSize;

		bool _canpost;
		unsigned char *_rawBuffer;

		inline int _intialize (const PicoloOpenParameters& params);
		inline int _uninitialize (void);

	protected:
		inline int _init (const PicoloOpenParameters& params);
		inline void _prepareBuffers (void);
};

///
//
//
double GetTimeAsSeconds(void)
{
	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()) + timev.usec() * 1e-6; 
}

void DelayInSeconds(double delay_in_seconds)
{
	ACE_Time_Value tv;
	tv.sec (int(delay_in_seconds));
	tv.usec ((delay_in_seconds-int(delay_in_seconds)) * 1e6);
	ACE_OS::sleep(tv);
}

///
/// full initialize and startup of the grabber.
//
inline int PicoloResources::_intialize (const PicoloOpenParameters& params)
{
	_init (params);
	open_bttvx();
	BttvxSetImageBuffer(params._unit_number,_rawBuffer);
	return YARP_OK;
}

inline int PicoloResources::_uninitialize (void)
{
	_bmutex.Wait ();
	close_bttvx();

	if (_rawBuffer != NULL) delete[] _rawBuffer;
	_rawBuffer = NULL;

	_nRequestedSize = 0;
	_nWidth = 0;
	_nHeight = 0;
	_nImageSize = 0;
	_canpost = true;

	_bmutex.Post ();

	return YARP_OK;
}

///
///
///
inline int PicoloResources::_init (const PicoloOpenParameters& params)
{
	/// copy params.
	int ret;
	_nRequestedSize = params._size_x;
	_nWidth = params._size_x;
	_nHeight = params._size_y;
	_nImageSize = params._size_x * params._size_y * 3;

	init_bttvx(params._video_type,params._unit_number,_nWidth,_nHeight, params._offset_x, params._offset_y);

	_bmutex.Wait ();
	_rawBuffer = new unsigned char [_nImageSize];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.Post ();

	return ret;
}

inline PicoloResources& RES(void *res) { return *(PicoloResources *)res; }

///
///
YARPPicoloDeviceDriver::YARPPicoloDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPPicoloDeviceDriver>(FCMDNCmds)
{
	system_resources = (void *) new PicoloResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[FCMDAcquireBuffer] = &YARPPicoloDeviceDriver::acquireBuffer;
	m_cmds[FCMDReleaseBuffer] = &YARPPicoloDeviceDriver::releaseBuffer;
	m_cmds[FCMDWaitNewFrame]  = &YARPPicoloDeviceDriver::waitOnNewFrame;
	m_cmds[FCMDGetSizeX]      = &YARPPicoloDeviceDriver::getWidth;
	m_cmds[FCMDGetSizeY]      = &YARPPicoloDeviceDriver::getHeight;
	m_cmds[FCMDSetBright]     = &YARPPicoloDeviceDriver::setBright;
	m_cmds[FCMDSetHue]        = &YARPPicoloDeviceDriver::setHue;
	m_cmds[FCMDSetContrast]   = &YARPPicoloDeviceDriver::setContrast;
	m_cmds[FCMDSetSatU]       = &YARPPicoloDeviceDriver::setSatU;
	m_cmds[FCMDSetSatV]       = &YARPPicoloDeviceDriver::setSatV;
	m_cmds[FCMDSetLNotch]	  = &YARPPicoloDeviceDriver::setLNotch;
	m_cmds[FCMDSetLDec]	  	  = &YARPPicoloDeviceDriver::setLDec;
	m_cmds[FCMDSetPeak]	  	  = &YARPPicoloDeviceDriver::setPeak;
	m_cmds[FCMDSetCagc]	  	  = &YARPPicoloDeviceDriver::setCagc;
	m_cmds[FCMDSetCkill]	  = &YARPPicoloDeviceDriver::setCkill;
	m_cmds[FCMDSetRange]	  = &YARPPicoloDeviceDriver::setRange;
	m_cmds[FCMDSetYsleep]	  = &YARPPicoloDeviceDriver::setYsleep;
	m_cmds[FCMDSetCsleep]	  = &YARPPicoloDeviceDriver::setCsleep;
	m_cmds[FCMDSetCrush]	  = &YARPPicoloDeviceDriver::setCrush;
	m_cmds[FCMDSetGamma]	  = &YARPPicoloDeviceDriver::setGamma;
	m_cmds[FCMDSetDithFrame]  = &YARPPicoloDeviceDriver::setDithFrame;
}

YARPPicoloDeviceDriver::~YARPPicoloDeviceDriver()
{
	if (system_resources != NULL)
		delete (PicoloResources *)system_resources;
	system_resources = NULL;
}

///
///
int YARPPicoloDeviceDriver::open (void *res)
{
	PicoloResources& d = RES(system_resources);
	int ret = d._intialize (*(PicoloOpenParameters *)res);

	return ret;
}

int YARPPicoloDeviceDriver::close (void)
{
	PicoloResources& d = RES(system_resources);

	int ret = d._uninitialize ();

	return ret;
}

int YARPPicoloDeviceDriver::acquireBuffer (void *buffer)
{
	PicoloResources& d = RES(system_resources);
	///d._bmutex.Wait ();
	BttvxAcquireBuffer(d._rawBuffer);
	(*(unsigned char **)buffer) = d._rawBuffer;

	return YARP_OK;
}

int YARPPicoloDeviceDriver::releaseBuffer (void *cmd)
{
	PicoloResources& d = RES(system_resources);
	BttvxReleaseBuffer();
	//d._canpost = true;
	//d._bmutex.Post ();

	return YARP_OK;
}

int YARPPicoloDeviceDriver::waitOnNewFrame (void *cmd)
{
	PicoloResources& d = RES(system_resources);
	BttvxWaitEvent();
	//d._new_frame.Wait ();
	///d._new_frame.acquire ();

	return YARP_OK;
}

int YARPPicoloDeviceDriver::getWidth (void *cmd)
{
	*(int *)cmd = RES(system_resources)._nWidth;
	return YARP_OK;
}

int YARPPicoloDeviceDriver::getHeight (void *cmd)
{
	*(int *)cmd = RES(system_resources)._nHeight;
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setBright(void *cmd)
{
	//	void bt848_bright(uint bright);
	unsigned int * m_bright = (unsigned int *) cmd;
	bt848_bright( *m_bright);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setHue(void *cmd)
{
	//	void bt848_hue(uint hue);
	unsigned int * m_hue = (unsigned int *) cmd;
	bt848_hue( *m_hue);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setContrast(void * cmd)
{
	//void bt848_contrast(uint cont);	
	unsigned int * m_contrast = (unsigned int *) cmd;
	bt848_contrast( *m_contrast);	
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setSatU(void * cmd)
{
	//void bt848_sat_u( ulong data);
	unsigned long * m_satu = (unsigned long *) cmd;
	bt848_sat_u( *m_satu);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setSatV(void * cmd)
{
	//void bt848_sat_v( ulong data);
	unsigned long * m_satv = (unsigned long *) cmd;
	bt848_sat_v( *m_satv);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setLNotch(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_lnotch(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setLDec(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_ldec(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setPeak(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_peak(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setCagc(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_cagc(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setCkill(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_ckill(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setRange(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_range(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setYsleep(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_ysleep(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setCsleep(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_csleep(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setCrush(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_crush(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setGamma(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_gamma(*m_state);
	return YARP_OK;
}

int
YARPPicoloDeviceDriver::setDithFrame(void * cmd)
{
	int * m_state = (int *) cmd;
	bt848_set_dithframe(*m_state);
	return YARP_OK;
}
