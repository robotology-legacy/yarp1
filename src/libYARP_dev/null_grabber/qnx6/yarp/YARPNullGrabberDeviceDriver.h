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
/// $Id: YARPNullGrabberDeviceDriver.h,v 1.1 2004-12-06 17:00:15 gmetta Exp $
///
///

#ifndef __YARPNullGrabberDeviceDriver__
#define __YARPNullGrabberDeviceDriver__

/**
 * \file YARPNullGrabberDeviceDriver.h dummy device driver to be used as replacement 
 * for real frame grabber-like devices. 
 */

#include <yarp/YARPConfig.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>

/**
 * Structure for defining the open() parameters of the null frame grabber. This are
 * completely fictious parameters. They're required for compatibility with higher
 * level classes, they're simply never used.
 */
struct NullGrabberOpenParameters
{
	int _unit_number;		/** board number 0, 1, 2, etc. */
	int _video_type;		/** 0 composite, 1 svideo. */
	int _size_x;			/** requested size x. */
	int _size_y;			/** requested size y. */
	int _offset_y;			/** y offset, with respect to the center. */
	int _offset_x;			/** x offset, with respect to the center. */
	float _alfa;			/** to have the possibility of shifting the ROI vertically, the requested size is actually _alfa*_size_y */
};


/**
 * The maximum number of messages allowed by the null driver.
 */
const int YARP_NULL_DD_CMDS = 256;

/**
 * A dummy device driver class. This can be used in place of a real device driver in cases where
 * the hardware is not available. Any call to this device driver will return 0 without complaints. 
 * Messages are simply ignored.
 */
class YARPNullGrabberDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPNullGrabberDeviceDriver>
{
private:
	YARPNullGrabberDeviceDriver(const YARPNullGrabberDeviceDriver&);
	void operator=(const YARPNullGrabberDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	YARPNullGrabberDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~YARPNullGrabberDeviceDriver();

	/**
	 * Opens the device driver.
	 * @param d is any pointer, the argument is not used of course.
	 * @return always returns YARP_OK.
	 */
	virtual int open(void *d);

	/**
	 * Closes the device driver.
	 * @return always returns YARP_OK.
	 */
	virtual int close(void);
};


#endif
