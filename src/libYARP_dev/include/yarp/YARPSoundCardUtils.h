// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPSoundCardUtils.h
// 
//     Description:  Commands for the sound card IOCtrl interface
// 
//         Version:  $Id: YARPSoundCardUtils.h,v 1.2 2004-07-13 13:21:07 babybot Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================

#ifndef __YARPSoundCardUtilsh__
#define __YARPSoundCardUtilsh__

#include <yarp/YARPConfig.h>

enum SoundCardCmd
{
	SCMDAcquireBuffer = 1,
	SCMDReleaseBuffer = 2,
	SCMDWaitNewFrame = 3,
	SCMDSetMute = 4,
	SCMDSetVolume = 5,
	SCMDNCmds = 6
};

#endif
