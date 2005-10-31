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
///                    #paulfitz#
///                    #emmebi#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPDragonflyDeviceDriver.cpp,v 1.2 2005-10-31 12:03:37 eshuy Exp $
///
///


#include "YARPDragonflyDeviceDriver.h"
#include <yarp/YARPFrameGrabberUtils.h>
//=============================================================================
// FlyCam Includes
//=============================================================================
//#include "../dd_orig/include/pgrflycapture.h"

#include "FirewireCameras.h"

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
	DragonflyResources (void) : _newFrameMutex(0),  _convImgMutex(1) 
	{
		// Variables initialization
		sizeX = 0;
		sizeY = 0;
		maxCams = 0;
		bufIndex = 0;
		_canPost = false;
		_acqStarted = false;
		_validContext = false;
		_pSubsampled_data = NULL;
		_raw_sizeX = 640;
		_raw_sizeY = 480;
	}

	~DragonflyResources () 
	{ 
		_uninitialize ();
		// To be sure - must protected against double calling
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
	

        FWCameras cam;
        YARPImageOf<YarpPixelRGB> img;

	YARPSemaphore mutexArray[_num_buffers];
	YARPSemaphore _newFrameMutex;
	YARPSemaphore _convImgMutex;
	
	inline int _initialize (const DragonflyOpenParameters& params);
	inline int _uninitialize (void);

	inline bool _setBrightness (int value, bool bDefault=false);
	inline bool _setExposure (int value, bool bDefault=false);
	inline bool _setWhiteBalance (int redValue, int blueValue, bool bDefault=false);
	inline bool _setShutter (int value, bool bDefault=false);
	inline bool _setGain (int value, bool bDefault=false);

	inline void _subSampling(void);

};

///
///
/// full initialize and startup of the grabber.
inline int DragonflyResources::_initialize (const DragonflyOpenParameters& params)
{
  cam.SetAuto(true);
  cam.Capture(img);
  sizeX = img.GetWidth();
  sizeY = img.GetHeight();
  
  return YARP_OK;
}

inline int DragonflyResources::_uninitialize (void)
{
  return YARP_OK;
}

///
///


inline bool DragonflyResources::_setBrightness (int value, bool bAuto)
{
  return false;
}

inline bool DragonflyResources::_setExposure (int value, bool bAuto)
{
  return false;
}

inline bool DragonflyResources::_setWhiteBalance (int redValue, int blueValue, bool bAuto)
{
  return false;
}

inline bool DragonflyResources::_setShutter (int value, bool bAuto)
{
  return false;
}

inline bool DragonflyResources::_setGain (int value, bool bAuto)
{
  return false;
}

inline void DragonflyResources::_subSampling(void)
{
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

	return ret;
}

int YARPDragonflyDeviceDriver::close (void)
{
	DragonflyResources& d = RES(system_resources);

	int ret = d._uninitialize ();

	return ret;
}

///
///
/// acquisition thread for real!
void YARPDragonflyDeviceDriver::Body (void)
{
  // actually we don't need this for this kind of camera
}

int YARPDragonflyDeviceDriver::acquireBuffer (void *buffer)
{
  DragonflyResources& d = RES(system_resources);
  
  (*(char **)buffer) = d.img.GetRawBuffer();
  d.sizeX = d.img.GetWidth();
  d.sizeY = d.img.GetHeight();
  
  return YARP_OK;
}

int YARPDragonflyDeviceDriver::releaseBuffer (void *cmd)
{	
  return YARP_OK;
}

int YARPDragonflyDeviceDriver::waitOnNewFrame (void *cmd)
{
  DragonflyResources& d = RES(system_resources);
  
  d.cam.Capture(d.img);
	
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
