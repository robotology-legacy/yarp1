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
/// $Id: YARPPicoloDeviceDriver.cpp,v 1.1 2003-05-30 17:28:28 gmetta Exp $
///
///

#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include "YARPPicoloDeviceDriver.h"
#include <YARPFrameGrabberUtils.h>

#define _WINDOWS
#include <sys/Picolo32.h>

class PicoloResources
{
public:
	PicoloResources (void) : _bmutex(1)
	{
		_nRequestedSize = 0;
		_nWidth = 0;
		_nHeight = 0;
		_nImageSize = 0;
		_picoloHandle = 0;

		memset (_bufHandles, 0, sizeof(PICOLOHANDLE) * _num_buffers);
		memset (_buffer, 0, sizeof(PUINT8) * _num_buffers);
		memset (_aligned, 0, sizeof(PUINT8) * _num_buffers);

		_rawBuffer = NULL;
	}

	~PicoloResources () { _uninitialize (); }

	enum { _num_buffers = 3 };

	PICOLOHANDLE _picoloHandle;	

	YARPSemaphore _bmutex;

	// Img size are determined partially by the HW.
	UINT32 _nRequestedSize;
	UINT32 _nWidth;
	UINT32 _nHeight;
	UINT32 _nImageSize;

	PICOLOHANDLE _bufHandles[_num_buffers];
	PUINT8 _buffer[_num_buffers];
	PUINT8 _aligned[_num_buffers];

	unsigned char *_rawBuffer;

	inline int _intialize (const PicoloOpenParameters& params);
	inline int _uninitialize (void);

protected:
	inline PICOLOHANDLE _init (const PicoloOpenParameters& params);
	inline void _prepareBuffers (void);
};


///
///
/// full initialize and startup of the grabber.
inline int PicoloResources::_intialize (const PicoloOpenParameters& params)
{
	_init (params);
	_prepareBuffers ();

	// n-part buffering.
	PICOLOHANDLE BufferList[_num_buffers];
	PICOLOSTATUS PicoloStatus;

	//
	int i;
	for (i = 0; i < _num_buffers; i++)
	{
		BufferList[i] = _bufHandles[i];
	}

	PicoloStatus = PicoloSetBufferList (_picoloHandle, BufferList, _num_buffers);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// select initial buffer.
	PicoloStatus = PicoloSelectBuffer(_picoloHandle, 0);

	///PicoloRegisterAcquisitionCallback(gdata.m_picoloHandle0, gdata.EndOfAcquisition0, &gdata);
	///PicoloRegisterAcquisitionCallback(gdata.m_picoloHandle1, gdata.EndOfAcquisition1, &gdata);

	// starts continuous acquisition.
	PicoloStatus = PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_CONTINUOUS | PICOLO_ACQUIRE_INC, 1);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	return YARP_OK;
}

inline int PicoloResources::_uninitialize (void)
{
	_bmutex.Wait ();
	
	if (_rawBuffer != NULL) delete[] _rawBuffer;
	_rawBuffer = NULL;
	
	_bmutex.Post ();

	return YARP_OK;
}


///
///
///
inline PICOLOHANDLE PicoloResources::_init (const PicoloOpenParameters& params)
{
	/// copy params.
	_nRequestedSize = params._size;

	/// starts board up.
	PICOLOHANDLE ret = PicoloStart(params._unit_number);
	if (ret < 0)
	{
		ACE_DEBUG ((LM_DEBUG, "troubles opening the grabber number %d\n", params._unit_number));
		return YARP_FAIL;
	}

	// video input.
	PICOLOSTATUS PicoloStatus;
	ACE_ASSERT (params._video_type == 0 || params._video_type == 1);

	/// it might require more params (e.g. ntsc, etc.)
	if (params._video_type == 0)
	{
		PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_COMPOSITE_BNC, PICOLO_IFORM_STD_PAL);
	}
	else
	{
		PicoloStatus = PicoloSelectVideoInput(ret, PICOLO_INPUT_SVIDEO_MINIDIN4, PICOLO_IFORM_625LINES);
	}

	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// image type. Color only?
	PicoloStatus = PicoloSelectImageFormat(ret, PICOLO_COLOR_RGB32);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// assume we want a square image
	float ratio = 576.0/_nRequestedSize;
	float xSize = 768.0/ratio;
	float offsetX = (xSize-_nRequestedSize) / 2;

	// adjust size and scaling. 
	PicoloStatus = PicoloSetControlFloat(ret,
										 PICOLO_CID_ADJUST_SCALEX,
										 ratio);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// img height is twice the requested size.
	PicoloStatus = PicoloSetControlFloat(ret,
										 PICOLO_CID_ADJUST_SCALEY,
										 ratio/2);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_SIZEX,
										 _nRequestedSize);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_SIZEY,
										 2 * _nRequestedSize);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_OFFSETX,
										 offsetX);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// re-get size (to be sure!)
	PicoloStatus = PicoloGetImageSize(ret, 
									  &_nWidth, 
									  &_nHeight);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	PicoloStatus = PicoloGetImageBufferSize(ret, &_nImageSize); 
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	_bmutex.Wait ();
	_rawBuffer = new unsigned char [_nImageSize];
	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.Post ();

	/// all ok, store the handle.
	_picoloHandle = ret;

	return ret;
}

///
///
inline void PicoloResources::_prepareBuffers(void)
{
	int i;
	for (i = 0; i < _num_buffers; i++)
	{
		_buffer[i] = (PUINT8)VirtualAlloc(NULL, 
										  _nImageSize, 
										  MEM_COMMIT, 
										  PAGE_READWRITE);
		ACE_ASSERT (_buffer[i] != NULL);
		_bufHandles[i] = PicoloSetImageBuffer(
									_picoloHandle, 
									_buffer[i],  
									_nImageSize, 
									PICOLO_FRAME_ANY, 
									(PVOID*) &_aligned[i]);
		_ASSERT (_bufHandles[i] >= 0);
		memset(_aligned[i], 0, _nImageSize);
	}
}

inline PicoloResources& RES(void *res) { return *(PicoloResources *)res; }

///
///
YARPPicoloDeviceDriver::YARPPicoloDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPPicoloDeviceDriver>(FCMDNCmds)
{
	system_resources = (void *) new PicoloResources;
	ACE_ASSERT (system_resources != NULL);
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

	Begin ();

	return ret;
}

int YARPPicoloDeviceDriver::close (void)
{
	PicoloResources& d = RES(system_resources);

	End ();	/// stops the thread first.

	int ret = d._uninitialize ();

	return ret;
}


///
///
/// acquisition thread for real!
void YARPPicoloDeviceDriver::Body (void)
{
	PICOLOSTATUS PicoloStatus;
	PicoloResources& d = RES(system_resources);

	PicoloStatus = PicoloSetWaitTimeout (d._picoloHandle, 120);		/// timeout 120 ms.
	bool finished = false;

	/// strategy, waits, copy into lockable buffer.
	while (!finished)	
	{
		PicoloStatus = PicoloWaitEvent (d._picoloHandle, PICOLO_EV_END_ACQUISITION);
		if (PicoloStatus == PICOLO_OK)
		{
			if (d._bmutex.PollingWait () == 1)
			{
				/// buffer acquired.
				/// read from buffer
				unsigned int bufno = 0;
				PicoloStatus = PicoloGetCurrentBuffer (d._picoloHandle, &bufno);
				if (PicoloStatus == PICOLO_OK)
				{
					memcpy (d._rawBuffer, d._aligned[bufno], d._nImageSize);
					d._bmutex.Post ();
				}
				else
				{
					/// does nothing
					ACE_DEBUG ((LM_DEBUG, "can't read the current buffer no : weird!\n"));
					d._bmutex.Post ();
				}
			}
			else
			{
				/// can't acquire, it means the buffer is still in use.
				/// silently ignores this condition.
			}
		}
		else
		{
			finished = true;
			ACE_DEBUG ((LM_DEBUG, "it's likely that the acquisition timed out, returning\n"));
		}
	}
}

int YARPPicoloDeviceDriver::acquireBuffer (unsigned char **buffer)
{
	PicoloResources& d = RES(system_resources);
	d._bmutex.Wait ();
	*buffer = d._rawBuffer;

	return YARP_OK;
}

int YARPPicoloDeviceDriver::releaseBuffer (void)
{
	PicoloResources& d = RES(system_resources);
	d._bmutex.Post ();

	return YARP_OK;
}

#undef _WINDOWS