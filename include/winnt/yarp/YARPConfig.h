
//
// $Id: YARPConfig.h,v 1.2 2004-07-09 16:42:32 gmetta Exp $
// YARP config file (template).
//
//

#ifndef __YARPConfigh__
#define __YARPConfigh__

//
// include here your config file and rename to YARPConfig.h

#define YARP_OS_CONFIG WIN32
#define YARP_ROBOT_CONFIG 1

/* OPERATING SYSTEM SUPPORT *******************************************/
#include <yarp/YARPConfigWin32.h>


/* ROBOT SUPPORT ******************************************************/
#ifdef YARP_ROBOT_CONFIG

#include <yarp/YARPConfigBabybot.h>
#include <yarp/YARPBabybotVocab.h>

#endif

#endif
