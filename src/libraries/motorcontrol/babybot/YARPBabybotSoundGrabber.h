// =====================================================================================
//
//       YARP - Yet Another Robotic Platform (c) 2001-2003 
//
//                    #Ing. Carlos Beltran#
//
//     "Licensed under the Academic Free License Version 1.0"
// 
//        Filename:  YARPBabybotSoundGrabber.h
// 
//     Description:  
// 
//         Version:  $Id: YARPBabybotSoundGrabber.h,v 1.1 2004-02-25 16:51:47 beltran Exp $
// 
//          Author:  Ing. Carlos Beltran (Carlos)
//         Company:  Lira-Lab
//           Email:  cbeltran@dist.unige.it
// 
// =====================================================================================
#ifndef __YARPBabybotSoundGrabberh__
#define __YARPBabybotSoundGrabberh__

#include <YARPGenericSoundGrabber.h>
#include <YARPSoundOnBabybotAdapter.h>

typedef YARPGenericSoundGrabber<YARPSoundOnBabybotAdapter, YARPBabybotSoundGrabberParams> YARPSoundGrabber;

//----------------------------------------------------------------------
// Note: I define directly the type YARPSoundGrabber and not YARPBabybotSoundDriver. At this level the name of
// the type can be the same in diferent architecture, is this right?. Thus we avoid a conditional compilation
// in higher levels (soundgrabber, framegrabber ...etc). 
//----------------------------------------------------------------------

#endif
