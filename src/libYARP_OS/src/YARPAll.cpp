/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa, paulfitz, nat#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPAll.cpp,v 2.0 2005-11-06 22:21:26 gmetta Exp $
///
///
#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/ACE.h>
#include <ace/OS.h>
#include <yarp/YARPNameService.h>

/**
 * \file YARPAll.cpp This file contains some initialization code and 
 * some old useful functions for synchronized output. This file is placed
 * into some library initialization code (when needed).
 */
#include <stdarg.h>

#define YARP_USE_OLD_PRINTF
#include <yarp/YARPAll.h>
#include <yarp/YARPSemaphore.h>

#ifdef __WIN32__
/// library initialization.
#pragma init_seg(lib)
#endif

static YARPSemaphore services_sema(1);


/**
 * This is a class that takes care of initializing ACE. It only contains
 * a constructor and destructor that are called when the library is
 * loaded/unloaded. Of course, the user is not concerned with this code.
 */ 
class YARPFooInitializer
{
public:
	/**
	 * Constructor.
	 */
	YARPFooInitializer () { 
	  ACE::init(); 
	  // by default won't see debug messages anymore
	  ACE_LOG_MSG->priority_mask (((ACE_LOG_MSG->priority_mask(ACE_Log_Msg::PROCESS))&(~LM_DEBUG))|LM_WARNING, ACE_Log_Msg::PROCESS);

	  ACE_OS::srand(ACE_OS::time(0)); 

#ifdef __DARWIN__
	  /*
	    There is a mysterious ACE? YARP? problem on Darwin,
	    that is fixed by the following code.
	    I either need to find out why, or wait
	    for the next version of ACE.
	   */
	  ACE_INET_Addr reg_addr;         /// who I am.
	  char myhostname[YARP_STRING_LEN] = "localhost";
	  getHostname (myhostname, YARP_STRING_LEN);
	  reg_addr.set((u_short)0, myhostname);
#endif
	}

	/**
	 * Destructor.
	 */
	~YARPFooInitializer () { ACE::fini(); }
} _fooinitializer;

/**
 * Initialization of the name service class.
 */
static YARPNameService _justtoinitialize;


/// -1 = disabled.
int __debug_level = -1;

/**
 * Sets the library debug level.
 * @param yarp is the YARP debug level (-1 means disabled).
 * @param ace is the ace debug level (e.g. whether ACE_DEBUG will print to stdout).
 */
void set_yarp_debug(int yarp, int ace) {
  if (yarp>=0) __debug_level = yarp;
  if (ace>0) {
    ACE_LOG_MSG->priority_mask (((ACE_LOG_MSG->priority_mask(ACE_Log_Msg::PROCESS))|(LM_DEBUG))|LM_WARNING, ACE_Log_Msg::PROCESS);
  } else {
    ACE_LOG_MSG->priority_mask (((ACE_LOG_MSG->priority_mask(ACE_Log_Msg::PROCESS))&(~LM_DEBUG))|LM_WARNING, ACE_Log_Msg::PROCESS);
  }
}

/** 
 * A thread-safe version of printf, using our private semaphore.
 * @param format is the format string as in printf.
 */
void YARP_safe_printf(char *format,...)
{
	va_list arglist;
	services_sema.Wait();
	va_start(arglist,format);
	vprintf(format,arglist);
	va_end(arglist);
	services_sema.Post();
}

/**
 * A thread-unsafe version of printf, in case the user likes to
 * Wait()/Post() the semaphore directly.
 * @param format is the format string as in printf.
 */
void YARP_unsafe_printf(char *format,...)
{
	va_list arglist;
	va_start(arglist,format);
	vprintf(format,arglist);
	va_end(arglist);
}

/**
 * Waits on the private output semaphore (the same used by YARP_safe_printf()).
 */
void YARP_output_wait()
{
	services_sema.Wait();
}

/**
 * Posts on the private output semaphore (the same used by YARP_safe_printf()).
 */
void YARP_output_post()
{
	services_sema.Post();
}
