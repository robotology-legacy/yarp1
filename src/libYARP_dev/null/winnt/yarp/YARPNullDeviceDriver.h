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
/// $Id: YARPNullDeviceDriver.h,v 1.1 2004-07-29 22:11:46 babybot Exp $
///
///

#ifndef __YARPNullDeviceDriver__
#define __YARPNullDeviceDriver__

/**
 * \file YARPNullDeviceDriver.h dummy device driver to be used as replacement for real devices. 
 */

#include <yarp/YARPConfig.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>

/**
 * The maximum number of messages allowed by the null driver.
 */
const int YARP_NULL_DD_CMDS = 256;

/**
 * A dummy device driver class. This can be used in place of a real device driver in cases where
 * the hardware is not available. Any call to this device driver will return 0 without complaints. 
 * Messages are simply ignored.
 */
class YARPNullDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPNullDeviceDriver>
{
private:
	YARPNullDeviceDriver(const YARPNullDeviceDriver&);
	void operator=(const YARPNullDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	YARPNullDeviceDriver() 
		: YARPDeviceDriver<YARPNullSemaphore, YARPNullDeviceDriver>(YARP_NULL_DD_CMDS)
	{}

	/**
	 * Destructor.
	 */
	virtual ~YARPNullDeviceDriver() {}

	/**
	 * Opens the device driver.
	 * @param d is any pointer, the argument is not used of course.
	 * @return always returns YARP_OK.
	 */
	virtual int open(void *d) { ACE_UNUSED_ARG(d); return YARP_OK; }

	/**
	 * Closes the device driver.
	 * @return always returns YARP_OK.
	 */
	virtual int close(void) { return YARP_OK; }
};


#endif