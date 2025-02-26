// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundOnBabybotAdapter.h
// 
//     Description:  
// 
//         Version:  $Id: YARPSoundOnBabybotAdapter.h,v 1.1 2004-07-28 17:17:35 babybot Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __YARPSoundOnBabybotAdapter__
#define __YARPSoundOnBabybotAdapter__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPSoundDeviceDriver.h>

typedef SoundOpenParameters YARPBabybotSoundGrabberParams;

// =====================================================================================
//        Class: YARPSoundOnBabybotAdater 
// 
//  Description:  
// 
//       Author:  Ing. Carlos Beltran
//      Created:  25/02/2004 11:31:17 W. Europe Standard Time
//     Revision:  none
// =====================================================================================
class YARPSoundOnBabybotAdapter : public YARPSoundDeviceDriver
{
public:
	YARPSoundOnBabybotAdapter() : YARPSoundDeviceDriver() {}
	virtual ~YARPSoundOnBabybotAdapter() {}

	int initialize (YARPBabybotSoundGrabberParams& params)
	{
		/// need additional initialization, put it here.
		return open ((void *)&params);
	}

	int uninitialize (void)
	{
		/// need additional termination stuff, here's the place for it.
		return close ();
	}
};
#endif
