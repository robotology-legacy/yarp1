/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
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
/// $Id: YARPEsdDeviceDriver.h,v 1.1 2006-02-21 16:25:21 gmetta Exp $
///
///

#ifndef __YARPEsdDeviceDriverh__
#define __YARPEsdDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

/**
 * \file YARPEsdDeviceDriver.h 
 * class for reading from the can bus, low level unstructured interface.
 */

/**
 * The open parameter class containing the initialization values.
 */
struct EsdDaqOpenParameters
{
	/**
	 * Constructor.
	 */
	EsdDaqOpenParameters (void)
	{
		_networkN = 0;
		_remote_address = 0;
		_my_address = 0;
		_polling_interval = 10;
		_timeout = 20;
		_scanSequence = 0;
		_p = NULL;

		_txQueueSize = 2047;					/** max len of the buffer for the esd driver */
		_rxQueueSize = 2047;
		_txTimeout = 20;						/** 20ms timeout */
		_rxTimeout = 20;
	}

	long int _txQueueSize;
	long int _rxQueueSize;
	long int _txTimeout;
	long int _rxTimeout;

	int _networkN;								/** network number */
	unsigned char _remote_address;				/** destination address */
	unsigned char _my_address;					/** my address */
	int _polling_interval;						/** thread polling interval [ms] */
	int _timeout;								/** number of cycles before timing out */
	int _scanSequence;							/** a bitmap representing the channel scan sequence */
	int (*_p) (const char *fmt, ...);			/** printf-like function for spying messages */
};


/**
 * The esd can device driver for polling the DAQ card.
 * Contains a thread that takes care of polling the can bus for incoming messages.
 * The class is derived from the standard YARP device driver base class.
 */
class YARPEsdDaqDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPEsdDaqDeviceDriver>, public YARPThread
{
private:
	YARPEsdDaqDeviceDriver(const YARPEsdDaqDeviceDriver&);
	void operator=(const YARPEsdDaqDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	YARPEsdDaqDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~YARPEsdDaqDeviceDriver();

	/**
	 * Opens the device driver.
	 * @param d is the pointer to the parameter structure which is expected to be
	 * of type EsdDaqOpenParameters.
	 * @return YARP_OK on success.
	 */ 
	virtual int open(void *d);

	/**
	 * Closes the device driver.
	 * @return YARP_OK on success.
	 */
	virtual int close(void);

protected:
	int scanSetup (void *cmd);
	int getMaxChannels (void *cmd);
	int aivReadScan (void *cmd);
	int aiReadScan (void *cmd);
	int aiReadChannel (void *cmd);
	int setDebugPrintFunction (void *cmd);

protected:
	void *system_resources;
	YARPSemaphore _mutex;
	YARPSemaphore _done;

	bool _writerequested;
	bool _noreply;
	
	/**
	 * pointer to the function printing the device debug information.
	 */
	int (*_p) (const char *fmt, ...);

	virtual void Body(void);

	/// helper functions
	int _writeWord16 (int msg, int axis, short s);
	int _writeWord16Ex (int msg, int axis, short s1, short s2);
	int _readWord16 (int msg, int axis, short& value);
	int _readWord16Array (int msg, short *out);
	int _readDWord (int msg, int axis, int& value);
	int _readDWordArray (int msg, int *out);
	int _writeDWord (int msg, int axis, int value);
	int _writeNone (int msg, int axis);

	enum { MAX_SHORT = 32767, MIN_SHORT = -32768, MAX_INT = 0x7fffffff, MIN_INT = 0x80000000 };
	enum { ESD_CAN_SKIP_ADDR = 0x80 };

	inline short S_16(double x) const 
	{
		if (x <= double(-(MAX_SHORT))-1)
			return MIN_SHORT;
		else
		if (x >= double(MAX_SHORT))
			return MAX_SHORT;
		else
			return short(x + .5);
	}

	inline int S_32(double x) const
	{
		if (x <= double(-(MAX_INT))-1.0)
			return MIN_INT;
		else
		if (x >= double(MAX_INT))
			return MAX_INT;
		else
			return int(x + .5);
	}
};



#endif