
//
// $Id: YARPConfig.h,v 1.3 2004-07-06 10:12:58 eshuy Exp $
// YARP config file (template).
//
//

#ifndef __YARPConfigh__
#define __YARPConfigh__

//
// include here your config file and rename to YARPConfig.h

/* OPERATING SYSTEM SUPPORT *******************************************/

// Add a line for each operating system supported.
// Select the operating system by externally defining
// YARP_OS_CONFIG as LINUX, WIN32 etc.
// The default in WIN32
#define YARP_OS_CONFIG_FILE_LINUX <yarp/conf/YARPConfigLinux.h>
#define YARP_OS_CONFIG_FILE_WIN32 <yarp/conf/YARPConfigWin32.h>

#define YARP_FORCE_EVAL_CONCAT(x) YARP_FORCE_EVAL_CONCAT2(x)
#define YARP_FORCE_EVAL_CONCAT2(x) YARP_OS_CONFIG_FILE_ ## x

#ifdef YARP_OS_CONFIG
#define YARP_OS_CONFIG_FILE YARP_FORCE_EVAL_CONCAT(YARP_OS_CONFIG)
#else
#define YARP_OS_CONFIG_FILE YARP_OS_CONFIG_FILE_WIN32
#endif

#include YARP_OS_CONFIG_FILE


/* ROBOT SUPPORT ******************************************************/

#ifdef YARP_ROBOT_CONFIG

#include <yarp/conf/YARPConfigBabybot.h>
#include <yarp/conf/YARPBabybotVocab.h>

#endif

#endif
