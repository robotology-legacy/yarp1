// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundDeviceDriver.h
// 
//     Description: 
// 
//         Version:  $Id: YARPSoundDeviceDriver.h,v 1.2 2004-07-13 13:21:10 babybot Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __YARPSoundDeviceDriverh__
#define __YARPSoundDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>

#include <stdlib.h>
#include <string.h>

// =====================================================================================
//        Class:  SoundOpenParamenters
// 
//  Description:  
// 
//       Author:  Ing. Carlos Beltran
//      Created:  18/02/2004 17:29:56 W. Europe Standard Time
//     Revision:  none
// =====================================================================================
struct SoundOpenParameters
{
	/// add here params for the open.
	SoundOpenParameters()
	{
		m_callbackthread_identifier = 0;
		m_Channels      = 0;
		m_SamplesPerSec = 0;
		m_BitsPerSample = 0;
		m_BufferLength  = 0;
	}

	int m_callbackthread_identifier;  //This is the thread identifier for the callback
	int m_Channels;
	int m_SamplesPerSec;
	int m_BitsPerSample;
	int m_BufferLength;
};

// =====================================================================================
//        Class:  YARPSoundDevideDriver
// 
//  Description:  
// 
//       Author:  Ing. Carlos Beltran
//      Created:  18/02/2004 17:31:33 W. Europe Standard Time
//     Revision:  none
// =====================================================================================
class YARPSoundDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPSoundDeviceDriver>, public YARPThread
{
private:
	YARPSoundDeviceDriver(const YARPSoundDeviceDriver&);
	void operator=(const YARPSoundDeviceDriver&);

public:
	YARPSoundDeviceDriver();
	virtual ~YARPSoundDeviceDriver();
	
	//----------------------------------------------------------------------
	//  overload open, close
	//----------------------------------------------------------------------
	virtual int open(void *d);
	virtual int close(void);

	//----------------------------------------------------------------------
	//  These functions should be implemented using the IOCtl mechanism
	//----------------------------------------------------------------------
	virtual int acquireBuffer(void *cmd);
	virtual int releaseBuffer(void *cmd);
	virtual int waitOnNewFrame (void *cmd);
	virtual int set_mute(void *cmd);
	virtual int set_volume(void *cmd);

protected:
	void *system_resources;

	virtual void Body(void);
};
#endif
