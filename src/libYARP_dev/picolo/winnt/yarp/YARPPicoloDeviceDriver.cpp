/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
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

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <ace/Sched_Params.h>

#include "YARPPicoloDeviceDriver.h"
#include <yarp/YARPFrameGrabberUtils.h>

#define _WINDOWS
#include "../dd_orig/include/Picolo32.h"

class PicoloResources
{
public:
	PicoloResources (void) : _bmutex(1), _new_frame(0)
	{
		_nRequestedSizeX = 0;
		_nRequestedSizeY = 0;
		_nWidth = 0;
		_nHeight = 0;
		_nImageSize = 0;
		_picoloHandle = 0;

		memset (_bufHandles, 0, sizeof(PICOLOHANDLE) * _num_buffers);
		memset (_buffer, 0, sizeof(PUINT8) * _num_buffers);
		memset (_aligned, 0, sizeof(PUINT8) * _num_buffers);

		_rawBuffer = NULL;
		_canpost = true;
	}

	~PicoloResources () { _uninitialize (); }

	enum { _num_buffers = 3 };

	PICOLOHANDLE _picoloHandle;	

	YARPSemaphore _bmutex;
	YARPSemaphore _new_frame;

	// Img size are determined partially by the HW.
	UINT32 _nRequestedSizeX;
	UINT32 _nRequestedSizeY;
	UINT32 _nWidth;
	UINT32 _nHeight;
	UINT32 _nImageSize;

	PICOLOHANDLE _bufHandles[_num_buffers];
	PUINT8 _buffer[_num_buffers];
	PUINT8 _aligned[_num_buffers];

	bool _canpost;

	unsigned char *_rawBuffer;

	inline int _intialize (const PicoloOpenParameters& params);
	inline int _uninitialize (void);

protected:
	inline PICOLOHANDLE _init (const PicoloOpenParameters& params);
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

	// starts continuous acquisition.
	PicoloStatus = PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_CONTINUOUS | PICOLO_ACQUIRE_INC, 1);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	return YARP_OK;
}

inline int PicoloResources::_uninitialize (void)
{
	_bmutex.Wait ();

	if (_nRequestedSizeX == 0 && _nRequestedSizeY == 0 && _nWidth == 0 && _nHeight == 0)
		return YARP_FAIL;

	PicoloAcquire (_picoloHandle, PICOLO_ACQUIRE_STOP, 1);
	PicoloStop (_picoloHandle);

	int i;
	for (i = 0; i < _num_buffers; i++)
		VirtualFree (_buffer[i], _nImageSize, MEM_DECOMMIT);

	if (_rawBuffer != NULL) delete[] _rawBuffer;
	_rawBuffer = NULL;

	_nRequestedSizeX = 0;
	_nRequestedSizeY = 0;
	_nWidth = 0;
	_nHeight = 0;
	_nImageSize = 0;
	_picoloHandle = 0;
	_canpost = true;
	
	_bmutex.Post ();

	return YARP_OK;
}


///
///
///
inline PICOLOHANDLE PicoloResources::_init (const PicoloOpenParameters& params)
{
	/// copy params.
	_nRequestedSizeX = params._size_x;
	_nRequestedSizeY = params._size_y;

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
	PicoloStatus = PicoloSelectImageFormat(ret, PICOLO_COLOR_RGB24);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// assume we want a square image
	float scalex = 768.0/_nRequestedSizeX;
	float scaley = (float) (576.0/(_nRequestedSizeY*params._alfa));	// a slighlty bigger immage is acquired, this allows some offsets along the vertical direction (see offset)
	float scale = (scalex < scaley) ? scalex : scaley;
	scalex = scale;
	scaley = scale / 2.0;	//the image is interlaced
	float xSize = 768.0/scalex;
	float ySize = 576.0/scaley;
	int offsetX = (int) ((xSize-_nRequestedSizeX) / 2 + params._offset_x + 0.5);
	int offsetY = (int) ((ySize-_nRequestedSizeY*2.0) / 2 + params._offset_y + 0.5);
	
	// adjust size and scaling. 
	PicoloStatus = PicoloSetControlFloat(ret,
										 PICOLO_CID_ADJUST_SCALEX,
										 scalex);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	// img height is twice the requested size.
	PicoloStatus = PicoloSetControlFloat(ret,
										 PICOLO_CID_ADJUST_SCALEY,
										 scaley);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_SIZEX,
										 _nRequestedSizeX);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_SIZEY,
										 2 * _nRequestedSizeY);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);

	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_OFFSETX,
										 offsetX);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	PicoloStatus = PicoloSetControlValue(ret,
										 PICOLO_CID_ADJUST_OFFSETY,
										 offsetY);
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
									PICOLO_FIELD_DOWN_ONLY,
									(PVOID*) &_aligned[i]);
		ACE_ASSERT (_bufHandles[i] >= 0);
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

	/// for the IOCtl call.
	m_cmds[FCMDAcquireBuffer] = &YARPPicoloDeviceDriver::acquireBuffer;
	m_cmds[FCMDReleaseBuffer] = &YARPPicoloDeviceDriver::releaseBuffer;
	m_cmds[FCMDWaitNewFrame] = &YARPPicoloDeviceDriver::waitOnNewFrame;
	m_cmds[FCMDGetSizeX] = &YARPPicoloDeviceDriver::getWidth;
	m_cmds[FCMDGetSizeY] = &YARPPicoloDeviceDriver::getHeight;
	m_cmds[FCMDSetBright] = &YARPPicoloDeviceDriver::setBrightness;
	m_cmds[FCMDSetHue] = &YARPPicoloDeviceDriver::setHue;
	m_cmds[FCMDSetContrast] = &YARPPicoloDeviceDriver::setContrast;
	m_cmds[FCMDSetSatU] = &YARPPicoloDeviceDriver::setSatU;
	m_cmds[FCMDSetSatV] = &YARPPicoloDeviceDriver::setSatV;
	m_cmds[FCMDSetLNotch] = &YARPPicoloDeviceDriver::setLNotch;
	m_cmds[FCMDSetLDec] = &YARPPicoloDeviceDriver::setLDec;
	m_cmds[FCMDSetCrush] = &YARPPicoloDeviceDriver::setCrush;

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

	End ();	/// stops the thread first (joins too).

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

	const int prio = ACE_Sched_Params::next_priority (ACE_SCHED_OTHER, GetPriority(), ACE_SCOPE_THREAD);
	SetPriority (prio);

	PicoloStatus = PicoloSetWaitTimeout (d._picoloHandle, 500);		/// timeout 120 ms.

	d._canpost = true;

	int i = 0;

	unsigned int bufno = 0;
	PicoloStatus = PicoloGetCurrentBuffer (d._picoloHandle, &bufno);
	ACE_ASSERT (PicoloStatus == PICOLO_OK);
	const unsigned int startbuf = (bufno > 0) ? (bufno - 1) : (d._num_buffers - 1);
	unsigned int readfro = startbuf;

	/// strategy, waits, copy into lockable buffer.
	while (!IsTerminated())	
	{
		PicoloStatus = PicoloWaitEvent (d._picoloHandle, PICOLO_EV_END_ACQUISITION);
		if (PicoloStatus != PICOLO_OK)
		{
			ACE_DEBUG ((LM_DEBUG, "it's likely that the acquisition timed out, returning\n"));
			ACE_DEBUG ((LM_DEBUG, "WARNING: this leaves the acquisition thread in an unterminated state --- can't be restarted from here!\n"));
			AskForEnd();
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

int YARPPicoloDeviceDriver::acquireBuffer (void *buffer)
{
	PicoloResources& d = RES(system_resources);
	d._bmutex.Wait ();
	(*(unsigned char **)buffer) = d._rawBuffer;

	return YARP_OK;
}

int YARPPicoloDeviceDriver::releaseBuffer (void *cmd)
{
	PicoloResources& d = RES(system_resources);
	d._canpost = true;
	d._bmutex.Post ();

	return YARP_OK;
}

int YARPPicoloDeviceDriver::waitOnNewFrame (void *cmd)
{
	PicoloResources& d = RES(system_resources);
	d._new_frame.Wait ();
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

int YARPPicoloDeviceDriver::setBrightness (void *cmd)
{
	/// RES(system_resources)._setBrightness(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setHue (void *cmd)
{
	/// RES(system_resources)._setHue(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setContrast (void *cmd)
{
	/// RES(system_resources)._setContrast(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setSatU (void *cmd)
{
	/// RES(system_resources)._setSatU(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setSatV (void *cmd)
{
	/// RES(system_resources)._setSatV(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setLNotch (void *cmd)
{
	/// RES(system_resources)._setLNotch(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setLDec (void *cmd)
{
	/// RES(system_resources)._setLDec(*cmd);
	return YARP_OK;
}

int YARPPicoloDeviceDriver::setCrush (void *cmd)
{
	/// RES(system_resources)._setCrush(*cmd);
	return YARP_OK;
}

#undef _WINDOWS