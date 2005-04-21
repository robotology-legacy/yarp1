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
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDragonflyDeviceDriver.cpp,v 1.2 2005-04-21 12:44:54 emmebi75 Exp $
///
///


#include "YARPDragonflyDeviceDriver.h"
#include <yarp/YARPFrameGrabberUtils.h>
//=============================================================================
// FlyCam Includes
//=============================================================================
#include "../dd_orig/include/pgrflycapture.h"

///
double GetTimeAs_mSeconds(void)
{
	ACE_Time_Value timev = ACE_OS::gettimeofday ();
	return double(timev.sec()*1e3) + timev.usec() * 1e-3; 
}

//
class DragonflyResources
{
public:
	DragonflyResources (void) : _newFrameMutex(0)/* CONV_MUTEX*/,  _convImgMutex(1) 
	{
		// Variables initialization
		sizeX = 0;
		sizeY = 0;
		maxCams = 0;
		bufIndex = 0;
		_canPost = false;
		imageConverted.pData = NULL;
		_acqStarted = false;
		_validContext = false;
		_pSubsampled_data = NULL;
		_raw_sizeX = 640;
		_raw_sizeY = 480;
	}

	~DragonflyResources () 
	{ 
		_uninitialize (); // To be sure - must protected against double calling
	}
	
	// Hardware-dependant variables
	enum { _num_buffers = 3 };
	int sizeX;
	int sizeY;
	int maxCams;
	int bufIndex;
	bool _canPost;
	bool _acqStarted;
	bool _validContext;
	int _raw_sizeX;
	int _raw_sizeY;
	unsigned char *_pSubsampled_data;
	

	FlyCaptureContext context;
	FlyCaptureImage imageBuffer[_num_buffers];
	FlyCaptureImage imageConverted;
	YARPSemaphore mutexArray[_num_buffers];
	YARPSemaphore _newFrameMutex;
	/* CONV_MUTEX*/
	YARPSemaphore _convImgMutex;
	
	inline int _initialize (const DragonflyOpenParameters& params);
	inline int _uninitialize (void);
	inline void _cleanBuffers (void);

	inline bool _setBrightness (int value, bool bDefault=false);
	inline bool _setExposure (int value, bool bDefault=false);
	inline bool _setWhiteBalance (int redValue, int blueValue, bool bDefault=false);
	inline bool _setShutter (int value, bool bDefault=false);
	inline bool _setGain (int value, bool bDefault=false);

	inline void _subSampling(void);

protected:
	inline void _prepareBuffers (void);
	inline void _destroyBuffers (void);

};

///
///
/// full initialize and startup of the grabber.
inline int DragonflyResources::_initialize (const DragonflyOpenParameters& params)
{
	FlyCaptureError   error = FLYCAPTURE_OK;

	// LATER: add a camera_init function

	sizeX = params._size_x;
	sizeY = params._size_y;

	// Create the context.
	if (!_validContext)
	{
		error = flycaptureCreateContext( &context );
		if (error != FLYCAPTURE_OK)
			return YARP_FAIL;
		_validContext = true;
		// Initialize the camera.
		error = flycaptureInitialize( context, params._unit_number );
		if (error != FLYCAPTURE_OK)
			return YARP_FAIL;
	}
	
	// Setup Camera Parameters, Magic Numbers :-)
	_setBrightness(0);
	_setExposure(300);
	_setWhiteBalance(20, 50); 
	_setShutter(320);	// x * 0.0625 = 20 mSec = 50 Hz
	_setGain(700);		// x * -0.0224 = -11.2dB
	
	// Set color reconstruction method
	error = flycaptureSetColorProcessingMethod(context, FLYCAPTURE_NEAREST_NEIGHBOR_FAST); // Should be an Option
	if (error != FLYCAPTURE_OK)
		return YARP_FAIL;

	// Set Acquisition Timeout
	error = flycaptureSetGrabTimeoutEx(context, 200);
	if (error != FLYCAPTURE_OK)
		return YARP_FAIL;
	// Buffers
	_prepareBuffers ();

	// Start Acquisition
	if (!_acqStarted)
	{
		error = flycaptureStart(	context, 
							FLYCAPTURE_VIDEOMODE_640x480Y8,
							FLYCAPTURE_FRAMERATE_30 );  
		if (error != FLYCAPTURE_OK)
			return YARP_FAIL;
		_acqStarted = true;
	}
	
	return YARP_OK;
}

inline int DragonflyResources::_uninitialize (void)
{
	int i;
	FlyCaptureError   error = FLYCAPTURE_OK;

	for (i=0; i< _num_buffers; i++)
		mutexArray[i].Wait();
	// Stop Acquisition
	if (_acqStarted)
	{
		error = flycaptureStop(context);
		if (error != FLYCAPTURE_OK)
			return YARP_FAIL;
		_acqStarted = false;
	}
	// Destroy the context.
	if (_validContext)
	{
		error = flycaptureDestroyContext( context );
		if (error != FLYCAPTURE_OK)
			return YARP_FAIL;
		_validContext = false;
	}
	// Deallocate buffers
	_destroyBuffers();
	for (i=0; i< _num_buffers; i++)
		mutexArray[i].Post();

	return YARP_OK;
}

///
///
inline void DragonflyResources::_prepareBuffers(void)
{
	if (imageConverted.pData == NULL)
		imageConverted.pData = new unsigned char[ _raw_sizeX * _raw_sizeY * 3 ];
	imageConverted.pixelFormat = FLYCAPTURE_BGR;
	if (_pSubsampled_data == NULL)
		_pSubsampled_data = new unsigned char[ sizeX * sizeY * 3 ];
	
	_cleanBuffers();
}

inline void DragonflyResources::_cleanBuffers(void)
{
	for (int i=0; i< _num_buffers; i++)
		memset( &imageBuffer[i], 0x0, sizeof( FlyCaptureImage ) );
	memset(_pSubsampled_data, 0x0, (sizeX * sizeY * 3));
}

inline void DragonflyResources::_destroyBuffers(void)
{
	if (imageConverted.pData != NULL)
		delete [] imageConverted.pData;
	imageConverted.pData = NULL;

	if (_pSubsampled_data != NULL)
		delete [] _pSubsampled_data;
	_pSubsampled_data = NULL;
}

inline bool DragonflyResources::_setBrightness (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_BRIGHTNESS, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setExposure (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_AUTO_EXPOSURE, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setWhiteBalance (int redValue, int blueValue, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_WHITE_BALANCE, redValue, blueValue, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setShutter (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_SHUTTER,value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline bool DragonflyResources::_setGain (int value, bool bAuto)
{
	FlyCaptureError   error = FLYCAPTURE_OK;
	error = flycaptureSetCameraProperty(context, FLYCAPTURE_GAIN, value, 0, bAuto);
	if (error == FLYCAPTURE_OK)
		return true;
	else 
		return false;
}

inline void DragonflyResources::_subSampling(void)
{

	int srcX, srcY;
	float xRatio, yRatio;
	int srcOffset;

	int srcSizeY = _raw_sizeY, srcSizeX = _raw_sizeX;
	int dstSizeY = sizeY, dstSizeX = sizeX;
	int bytePerPixel = 3;

	xRatio = ((float)srcSizeX)/dstSizeX;
	yRatio = ((float)srcSizeY)/dstSizeY;

	unsigned char *pSrcImg = imageConverted.pData;
	unsigned char *pDstImg = _pSubsampled_data;

	unsigned char *pSrc = pSrcImg;
	unsigned char *pDst = pDstImg;

	for (int j=0; j<dstSizeY; j++)
	{
		srcY = (int)(yRatio*j);
		srcOffset = srcY * srcSizeX * bytePerPixel;

		for (int i=0; i<dstSizeX; i++)
		{
			srcX = (int)(xRatio*i);
			pSrc = pSrcImg + srcOffset + (srcX * bytePerPixel);
			memcpy(pDst,pSrc,bytePerPixel);
			pDst += bytePerPixel;

		}
	}
}

inline DragonflyResources& RES(void *res) { return *(DragonflyResources *)res; }

///
///
YARPDragonflyDeviceDriver::YARPDragonflyDeviceDriver(void) : YARPDeviceDriver<YARPNullSemaphore, YARPDragonflyDeviceDriver>(FCMDNCmds)
{
	system_resources = (void *) new DragonflyResources;
	ACE_ASSERT (system_resources != NULL);

	/// for the IOCtl call.
	m_cmds[FCMDAcquireBuffer] = &YARPDragonflyDeviceDriver::acquireBuffer;
	m_cmds[FCMDReleaseBuffer] = &YARPDragonflyDeviceDriver::releaseBuffer;
	m_cmds[FCMDWaitNewFrame] = &YARPDragonflyDeviceDriver::waitOnNewFrame;
	m_cmds[FCMDGetSizeX] = &YARPDragonflyDeviceDriver::getWidth;
	m_cmds[FCMDGetSizeY] = &YARPDragonflyDeviceDriver::getHeight;
	m_cmds[FCMDSetBright] = &YARPDragonflyDeviceDriver::setBrightness;
	m_cmds[FCMDSetHue] = &YARPDragonflyDeviceDriver::setHue;
	m_cmds[FCMDSetContrast] = &YARPDragonflyDeviceDriver::setContrast;
	m_cmds[FCMDSetSatU] = &YARPDragonflyDeviceDriver::setSatU;
	m_cmds[FCMDSetSatV] = &YARPDragonflyDeviceDriver::setSatV;
	m_cmds[FCMDSetLNotch] = &YARPDragonflyDeviceDriver::setLNotch;
	m_cmds[FCMDSetLDec] = &YARPDragonflyDeviceDriver::setLDec;
	m_cmds[FCMDSetCrush] = &YARPDragonflyDeviceDriver::setCrush;

}

YARPDragonflyDeviceDriver::~YARPDragonflyDeviceDriver()
{
	if (system_resources != NULL)
		delete (DragonflyResources *)system_resources;
	system_resources = NULL;
}

///
///
int YARPDragonflyDeviceDriver::open (void *res)
{
	DragonflyResources& d = RES(system_resources);
	int ret = d._initialize (*(DragonflyOpenParameters *)res);
	YARPScheduler::setHighResScheduling ();
	Begin ();

	return ret;
}

int YARPDragonflyDeviceDriver::close (void)
{
	DragonflyResources& d = RES(system_resources);

	End ();	/// stops the thread first (joins too).

	int ret = d._uninitialize ();

	return ret;
}

///
///
/// acquisition thread for real!
void YARPDragonflyDeviceDriver::Body (void)
{
	DragonflyResources& d = RES(system_resources);

	FlyCaptureError   error = FLYCAPTURE_OK;

	d._cleanBuffers();
	d.bufIndex = 0;
	d._canPost = true;

	int nImages = 0;
	double startTime;
	startTime = GetTimeAs_mSeconds();
	int lostFrames = 0;

	ACE_DEBUG ((LM_DEBUG, "\n[DragonFly Driver] Acquisition thread starting... "));
	while (!IsTerminated())	
	{
		d.bufIndex++;
	
		if (d.bufIndex == d._num_buffers)
				d.bufIndex = 0;
		
		if ((d.mutexArray[d.bufIndex]).PollingWait() == 1)
		{

			error = flycaptureGrabImage2( d.context, &(d.imageBuffer[d.bufIndex]) );
			if (d._canPost)
			{
				d._canPost = false;
				d._newFrameMutex.Post();
			}
			
			(d.mutexArray[d.bufIndex]).Post();
			if (error != FLYCAPTURE_OK)
			{
				ACE_DEBUG ((LM_DEBUG, "\n[DragonFly Driver] ERROR: It's likely that the acquisition failed, returning. "));
				AskForEnd();
			}
			nImages++;
		}
		else
		{
			ACE_DEBUG ((LM_DEBUG, "\n[DragonFly Driver] WARNING: Frame lost. "));
			lostFrames++;
		}
	}
	
	double stopTime;
	stopTime = GetTimeAs_mSeconds();
	unsigned int delay = int(stopTime - startTime);
	unsigned int avTime = delay / nImages;
	double fps = 1000 * (1/double(avTime));
	char str[256];
	sprintf(str,"\n[DragonFly Driver] Acquired %u frames(s) in %ums (average time %ums per image, %.2ffps). ", nImages, delay, avTime, fps);
	ACE_DEBUG ((LM_DEBUG, str));
	sprintf(str,"\n[DragonFly Driver] %u lost Frame(s). ");
	ACE_DEBUG ((LM_DEBUG, "\n[DragonFly Driver] Acquisition thread returning... "));
}

int YARPDragonflyDeviceDriver::acquireBuffer (void *buffer)
{
	DragonflyResources& d = RES(system_resources);

	FlyCaptureError   error = FLYCAPTURE_OK;

	int reqBuffer = d.bufIndex - 1;
	if (reqBuffer < 0)
		reqBuffer = d._num_buffers - 1;

	(d.mutexArray[reqBuffer]).Wait ();
	/* CONV_MUTEX*/
	d._convImgMutex.Wait();
	 
	FlyCaptureImage *pSrc;
	FlyCaptureImage *pDst;

	pDst = &(d.imageConverted);
	pSrc = &(d.imageBuffer[reqBuffer]);

	error = flycaptureConvertImage( d.context, pSrc, pDst );

	(d.mutexArray[reqBuffer]).Post ();

	/*
	if (error != FLYCAPTURE_OK)
		return YARP_FAIL;
	*/
	//unsigned char * pData = (*(unsigned char **)buffer);

	// Copy or just reference??
	//memcpy (pData, d.imageConverted.pData, (d.sizeX * d.sizeY * 3) );
	if ((d.sizeX == d._raw_sizeX) && (d.sizeY == d._raw_sizeY) )
		(*(unsigned char **)buffer)= d.imageConverted.pData;
	else
	{
		d._subSampling();
		(*(unsigned char **)buffer)= d._pSubsampled_data;
	}
	
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::releaseBuffer (void *cmd)
{
	DragonflyResources& d = RES(system_resources);

	d._canPost = true;
	/* CONV_MUTEX*/ 
	d._convImgMutex.Post();
	
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::waitOnNewFrame (void *cmd)
{
	DragonflyResources& d = RES(system_resources);
	
	d._newFrameMutex.Wait ();
	
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::getWidth (void *cmd)
{
	*(int *)cmd = RES(system_resources).sizeX;
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::getHeight (void *cmd)
{
	*(int *)cmd = RES(system_resources).sizeY;
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setBrightness (void *cmd)
{
	/// RES(system_resources)._setBrightness(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setHue (void *cmd)
{
	/// RES(system_resources)._setHue(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setContrast (void *cmd)
{
	/// RES(system_resources)._setContrast(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setSatU (void *cmd)
{
	/// RES(system_resources)._setSatU(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setSatV (void *cmd)
{
	/// RES(system_resources)._setSatV(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setLNotch (void *cmd)
{
	/// RES(system_resources)._setLNotch(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setLDec (void *cmd)
{
	/// RES(system_resources)._setLDec(*cmd);
	return YARP_OK;
}

int YARPDragonflyDeviceDriver::setCrush (void *cmd)
{
	/// RES(system_resources)._setCrush(*cmd);
	return YARP_OK;
}
