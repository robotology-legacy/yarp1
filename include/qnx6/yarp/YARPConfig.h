
//
// $Id: YARPConfig.h,v 1.1 2004-07-10 23:17:34 gmetta Exp $
// YARP config file (template).
//
//

#ifndef __YARPConfigh__
#define __YARPConfigh__

//
// include here your config file and rename to YARPConfig.h

#define YARP_OS_CONFIG QNX6
#define YARP_ROBOT_CONFIG 1

/* OPERATING SYSTEM SUPPORT *******************************************/
#include <yarp/YARPConfigQNX6.h>


/* ROBOT SUPPORT ******************************************************/
#ifdef YARP_ROBOT_CONFIG

#include <yarp/YARPConfigNullrobot.h>
#include <yarp/YARPBabybotVocab.h>

#endif

#endif
