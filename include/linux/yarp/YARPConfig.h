
//
// $Id: YARPConfig.h,v 1.3 2005-02-24 22:12:08 natta Exp $
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
#define YARP_OS_CONFIG_FILE_LINUX <yarp/YARPConfigLinux.h>
#define YARP_OS_CONFIG_FILE_WIN32 <yarp/YARPConfigWin32.h>

#define YARP_FORCE_EVAL_CONCAT(x) YARP_FORCE_EVAL_CONCAT2(x)
#define YARP_FORCE_EVAL_CONCAT2(x) YARP_OS_CONFIG_FILE_ ## x

#ifdef YARP_OS_CONFIG
#define YARP_OS_CONFIG_FILE YARP_FORCE_EVAL_CONCAT(YARP_OS_CONFIG)
#else
#ifdef __linux__
#define YARP_OS_CONFIG_FILE YARP_OS_CONFIG_FILE_LINUX
#else
#define YARP_OS_CONFIG_FILE YARP_OS_CONFIG_FILE_WIN32
#endif
#endif

#include YARP_OS_CONFIG_FILE


/* ROBOT SUPPORT ******************************************************/

#ifdef YARP_ROBOT_CONFIG

#include <yarp/YARPConfigBabybot.h>
#include <yarp/YARPBabybotVocab.h>

#endif

#endif
