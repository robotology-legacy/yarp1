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
/// $Id: YARPPicoloDeviceDriver.cpp,v 1.9 2003-07-03 09:27:20 beltran Exp $
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
		//_picoloHandle = 0;

		//memset (_bufHandles, 0, sizeof(PICOLOHANDLE) * _num_buffers);
		//memset (_buffer, 0, sizeof(PUINT8) * _num_buffers);
		//memset (_aligned, 0, sizeof(PUINT8) * _num_buffers);

		_rawBuffer = NULL;
		_canpost = true;
	}

	~PicoloResources () { _uninitialize (); }

	enum { _num_buffers = 3 };

	//PICOLOHANDLE _picoloHandle;	

	YARPSemaphore _bmutex;
	YARPSemaphore _new_frame;

	// Img size are determined partially by the HW.
	UINT32 _nRequestedSize;
	UINT32 _nWidth;
	UINT32 _nHeight;
	UINT32 _nImageSize;

	///PICOLOHANDLE _bufHandles[_num_buffers];
	///PUINT8 _buffer[_num_buffers];
	///PUINT8 _aligned[_num_buffers];

	bool _canpost;

	unsigned char *_rawBuffer;

	inline int _intialize (const PicoloOpenParameters& params);
	inline int _uninitialize (void);

protected:
	inline int _init (const PicoloOpenParameters& params);
	inline void _prepareBuffers (void);
};

///
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
///
/// full initialize and startup of the grabber.
inline int PicoloResources::_intialize (const PicoloOpenParameters& params)
{
	_init (params);
	open_bttvx();
	BttvxSetImageBuffer(params._unit_number,_rawBuffer);
	//_prepareBuffers ();

	// n-part buffering.
	///PICOLOHANDLE BufferList[_num_buffers];
	////PICOLOSTATUS PicoloStatus;

	//
	///int i;
	////for (i = 0; i < _num_buffers; i++)
	///{
	///	BufferList[i] = _bufHandles[i];
	///}

	////PicoloStatus = PicoloSetBufferList (_picoloHandle, BufferList, _num_buffers);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// select initial buffer.
	////PicoloStatus = PicoloSelectBuffer(_picoloHandle, 0);

	// starts continuous acquisition.
	///PicoloStatus = PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_CONTINUOUS | PICOLO_ACQUIRE_INC, 1);
	///ACE_ASSERT (PicoloStatus == PICOLO_OK);

	return YARP_OK;
}

inline int PicoloResources::_uninitialize (void)
{
	_bmutex.Wait ();

	//if (_nRequestedSize == 0 && _nWidth == 0 && _nHeight == 0)
	///	return YARP_FAIL;

	///PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_STOP, 1);
	///PicoloStop (_picoloHandle);
	/*

	int i;
	for (i = 0; i < _num_buffers; i++)
		VirtualFree (_buffer[i], _nImageSize, MEM_DECOMMIT);
	*/
	if (_rawBuffer != NULL) delete[] _rawBuffer;
	_rawBuffer = NULL;

	_nRequestedSize = 0;
	_nWidth = 0;
	_nHeight = 0;
	_nImageSize = 0;
	///_picoloHandle = 0;
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
	_nRequestedSize = params._size;
	_nWidth = params._size;
	_nHeight = params._size;
	_nImageSize = params._size * params._size * 3;
	
	//Attention: the size must me dinamic in the driver!!! TODO
	
	init_bttvx(params._video_type,params._unit_number,_nWidth,_nHeight);
	/// starts board up.
	////PICOLOHANDLE ret = PicoloStart(params._unit_number);
	////if (ret < 0)
	///{
	///	ACE_DEBUG ((LM_DEBUG, "troubles opening the grabber number %d\n", params._unit_number));
	///	return YARP_FAIL;
	///}

	// video input.
	////PICOLOSTATUS PicoloStatus;
	////ACE_ASSERT (params._video_type == 0 || params._video_type == 1);

	/// it might require more params (e.g. ntsc, etc.)
	////if (params._video_type == 0)
	////{
	////	PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_COMPOSITE_BNC, PICOLO_IFORM_STD_PAL);
	////}
	////else
	////{
	////	PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_SVIDEO_MINIDIN4, PICOLO_IFORM_625LINES);
	////}

	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// image type. Color only?
	////PicoloStatus = PicoloSelectImageFormat(ret, PICOLO_COLOR_RGB32);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// assume we want a square image
	////float scalex = 576.0/_nRequestedSize;
	////float scaley = scalex / 2.0;
	////float xSize = 768.0/scalex;
	////float offsetX = (xSize-_nRequestedSize) / 2;

	// adjust size and scaling. 
	////PicoloStatus = PicoloSetControlFloat(ret,
	////									 PICOLO_CID_ADJUST_SCALEX,
	////									 scalex);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// img height is twice the requested size.
	////PicoloStatus = PicoloSetControlFloat(ret,
	////									 PICOLO_CID_ADJUST_SCALEY,
	////									 scaley);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);
	////PicoloStatus = PicoloSetControlValue(ret,
	////									 PICOLO_CID_ADJUST_SIZEX,
	////									 _nRequestedSize);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);
	////PicoloStatus = PicoloSetControlValue(ret,
	////									 PICOLO_CID_ADJUST_SIZEY,
	////									 2 * _nRequestedSize);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	////PicoloStatus = PicoloSetControlValue(ret,
	////									 PICOLO_CID_ADJUST_OFFSETX,
	////									 offsetX);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// re-get size (to be sure!)
	////PicoloStatus = PicoloGetImageSize(ret, 
	////								  &_nWidth, 
	////								  &_nHeight);
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	////PicoloStatus = PicoloGetImageBufferSize(ret, &_nImageSize); 
	////ACE_ASSERT (PicoloStatus == PICOLO_OK);

	_bmutex.Wait ();
	_rawBuffer = new unsigned char [_nImageSize];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.Post ();

	/// all ok, store the handle.
	//_picoloHandle = ret;
	////_picoloHandle = params._unit_number;
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
	m_cmds[FCMDWaitNewFrame] = &YARPPicoloDeviceDriver::waitOnNewFrame;
	m_cmds[FCMDGetSizeX] = &YARPPicoloDeviceDriver::getWidth;
	m_cmds[FCMDGetSizeY] = &YARPPicoloDeviceDriver::getHeight;
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

	///Begin ();

	return ret;
}

int YARPPicoloDeviceDriver::close (void)
{
	PicoloResources& d = RES(system_resources);

	////End ();	/// stops the thread first.

	int ret = d._uninitialize ();

	return ret;
}

///
///
/// acquisition thread for real!
/***
void YARPPicoloDeviceDriver::Body (void)
{
	PICOLOSTATUS PicoloStatus;
	PicoloResources& d = RES(system_resources);

	const int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	SetPriority (prio);

	PicoloStatus = PicoloSetWaitTimeout (d._picoloHandle, 500);		/// timeout 120 ms.
	bool finished = false;

	d._canpost = true;

	int i = 0;

	unsigned int bufno = 0;
	PicoloStatus = PicoloGetCurrentBuffer (d._picoloHandle, &bufno);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	const unsigned int startbuf = (bufno > 0) ? (bufno - 1) : (d._num_buffers - 1);
	unsigned int readfro = startbuf;

	/// strategy, waits, copy into lockable buffer.
	while (!finished)	
	{
		PicoloStatus = PicoloWaitEvent (d._picoloHandle, PICOLO_EV_END_ACQUISITION);
		if (PicoloStatus != PICOLO_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "it's likely that the acquisition timed out, returning\n"));
			finished = true;
		}

		readfro = startbuf;

		for (i = 0; i < d._num_buffers; i++)
		{
			if (d._bmutex.PollingWait () == 1)
			{
				/// buffer acquired.
				/// read from buffer
				memcpy (d._rawBuffer, d._aligned[readfro], d._nImageSize);
					
				if (d._canpost)
				{
					d._canpost = false;
					d._new_frame.Post();
				}

				d._bmutex.Post ();
			}
			else
			{
				/// can't acquire, it means the buffer is still in use.
				/// silently ignores this condition.
				ACE_DEBUG ((LM_DEBUG, "lost a frame, acq thread\n"));
			}

			readfro = ((readfro + 1) % d._num_buffers);

			/// 40 ms delay
			if (i < d._num_buffers-1)
				DelayInSeconds (0.040);
		} /// end for
	}

	ACE_DEBUG ((LM_DEBUG, "acquisition thread returning...\n"));
}
*/

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

