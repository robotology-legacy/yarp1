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
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPNullGrabberOnRobotcubAdapter.h,v 1.3 2006-02-03 00:00:08 gmetta Exp $
///
///

#ifndef __YARPNullGrabberOnRobotcubAdapter__
#define __YARPNullGrabberOnRobotcubAdapter__

#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPNullGrabberDeviceDriver.h>

/// don't need anything fancier.
typedef NullGrabberOpenParameters YARPRobotcubGrabberParams;

///
///
///
class YARPNullGrabberOnRobotcubAdapter : public YARPNullGrabberDeviceDriver
{
public:
	YARPNullGrabberOnRobotcubAdapter() : YARPNullGrabberDeviceDriver() {}
	virtual ~YARPNullGrabberOnRobotcubAdapter() {}

	int initialize (YARPRobotcubGrabberParams& params)
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

