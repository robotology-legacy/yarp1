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

//
// Define a common interface  for device drivers
//
// 
// feb 2003 -- by nat and pasa
// $Id: YARPDeviceDriver.h,v 1.2 2004-07-13 13:21:07 babybot Exp $

#ifndef __YARP_DEVICE_DRIVER__
#define __YARP_DEVICE_DRIVER__

#include <yarp/YARPConfig.h>

/**
 * \file YARPDeviceDriver.h must be compilable across multiple architectures (NT, Linux, QNX).
 */

/**
 * Template class defining the common interface to yarp hardware. The template takes two
 * arguments, a SYNC object (YARPSemaphore for instance), and the class actual device
 * drivers are derived from.
 */
template <class SYNC, class DERIVED>
class YARPDeviceDriver
{
public:
	/**
	 * Constructor.
	 * @param n_cmds is the number of different messages the driver should be dealing with.
	 */
	YARPDeviceDriver(int n_cmds)
	{
		m_cmds = (cmd_function_t *)new char[n_cmds*sizeof(cmd_function_t)];

		for(int i = 0; i < n_cmds; i++)
			m_cmds[i] = &DERIVED::defaultCmd;

		m_handle = 0;
	}

	/**
	 * Destructor.
	 */
	virtual ~YARPDeviceDriver()
	{
		delete [] m_cmds;

		if (m_handle != 0)
			((DERIVED *) this)->close();
	}

protected:

	void *m_handle;

	typedef int (DERIVED::*cmd_function_t)(void *);

	cmd_function_t *m_cmds;

	/**
	 * Default function for unhandled messages.
	 * @param p is the pointer to the argument.
	 * @return always 0.
	 */
	int defaultCmd (void *p)
	{
		// default function (for safety)
		return 0;
	}

	/**
	 * Waits on the synchronization object.
	 */
	inline void lock(void) { m_mutex.Wait(); }

	/**
	 * Releases the synchronization object.
	 */
	inline void unlock(void) { m_mutex.Post(); }

	SYNC m_mutex;

public:
	/**
	 * Opens the device driver.
	 * @param p is a pointer to the open arguments.
	 * @return it should return YARP_OK on success.
	 */
	virtual int open(void *p) = 0;

	/**
	 * Closes the device driver.
	 * @return is should return YARP_OK on success.
	 */
	virtual int close() = 0;

	/**
	 * This is the only access to the device driver functions.
	 * @param cmd is the identifier of the message.
	 * @param data is the pointer to the arguments to the function call.
	 * @return the same value returned by the call to the function handling the message.
	 */
	int IOCtl(int cmd, void *data)
	{
		lock();

		int ret = ((DERIVED *)this->*m_cmds[cmd])(data);

		unlock();
		return ret;
	}
};

#endif
