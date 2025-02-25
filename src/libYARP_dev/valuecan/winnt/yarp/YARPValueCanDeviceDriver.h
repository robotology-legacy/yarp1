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
/// $Id: YARPValueCanDeviceDriver.h,v 1.9 2005-04-17 08:12:23 babybot Exp $
///
///

#ifndef __YARPValueCanDeviceDriverh__
#define __YARPValueCanDeviceDriverh__

#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPThread.h>
#include <yarp/YARPTime.h>

/**
 * \file YARPValueCanDeviceDriver.h 
 * class for interfacing with the value can device driver.
 */

/**
 * Max number of addressable cards in this implementation.
 */
const int VALUE_MAX_CARDS		= 16;

/**
 * The open parameter class containing the initialization values.
 */
struct ValueCanOpenParameters
{
	/**
	 * Constructor.
	 */
	ValueCanOpenParameters (void)
	{
		memset (_destinations, 0, sizeof(unsigned char) * VALUE_MAX_CARDS);
		_my_address = 0;
		_polling_interval = 10;
		_timeout = 2;
		_njoints = 0;
		_p = NULL;
	}

	int _njoints;								/** number of joints (cards * 2) */
	unsigned char _destinations[VALUE_MAX_CARDS];		/** destination addresses */
	unsigned char _my_address;					/** my address */
	int _polling_interval;						/** thread polling interval [ms] */
	int _timeout;								/** number of cycles before timing out */
	int (*_p) (char *fmt, ...);					/** printf-like function for spying messages */
};


/**
 * The value can device driver.
 * Contains a thread that takes care of polling the can bus for incoming messages.
 * The class is derived from the standard YARP device driver base class.
 */
class YARPValueCanDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPValueCanDeviceDriver>, public YARPThread
{
private:
	YARPValueCanDeviceDriver(const YARPValueCanDeviceDriver&);
	void operator=(const YARPValueCanDeviceDriver&);

public:
	/**
	 * Constructor.
	 */
	YARPValueCanDeviceDriver();

	/**
	 * Destructor.
	 */
	virtual ~YARPValueCanDeviceDriver();

	/**
	 * Opens the device driver.
	 * @param d is the pointer to the parameter structure which is expected to be
	 * of type ValueCanOpenParameters.
	 * @return YARP_OK on success.
	 */ 
	virtual int open(void *d);

	/**
	 * Closes the device driver.
	 * @return YARP_OK on success.
	 */
	virtual int close(void);

protected:
	int getPosition(void *cmd);
	int getPositions(void *cmd);
	int getRefPosition (void *cmd);
	int getRefPositions(void *cmd);
	int setPosition(void *cmd);
	int setPositions(void *cmd);
	int getError(void *cmd);
	int setSpeed(void *cmd);
	int setSpeeds(void *cmd);
	int getSpeeds(void *cmd);
	int getRefSpeeds(void *cmd);
	int setAcceleration(void *cmd);
	int setAccelerations(void *cmd);
	int getRefAccelerations(void *cmd);
	int setOffset(void *cmd);
	int setOffsets(void *cmd);
	int setPid(void *cmd);
	int getPid(void *cmd);
	int setIntegratorLimit(void *cmd);
	int setIntegratorLimits(void *cmd);
	int definePosition(void *cmd);
	int definePositions(void *cmd);
	int enableAmp(void *cmd);
	int disableAmp(void *cmd);
	int controllerIdle(void *cmd);
	int controllerRun(void *cmd);
	int velocityMove(void *cmd);
	int setCommand(void *cmd);
	int setCommands(void *cmd);
	int getTorque(void *cmd);
	int getTorques(void *cmd);
	int readBootMemory(void *cmd);
	int writeBootMemory(void *cmd);
	int setSwPositiveLimit(void *cmd);
	int setSwNegativeLimit(void *cmd);
	int getSwPositiveLimit(void *cmd);
	int getSwNegativeLimit(void *cmd);
	int setTorqueLimit (void *cmd);
	int setTorqueLimits (void *cmd);
	int getTorqueLimit (void *cmd);
	int getTorqueLimits (void *cmd);
	int getErrorStatus (void *cmd);
	int checkMotionDone (void *cmd);

	int setDebugMessageFilter (void *cmd);
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
	int (*_p) (char *fmt, ...);

	/**
	 * filter for recurrent messages.
	 */
	int _filter;

	/**
	 * helper function to check whether the enabled flag is on or off.
	 * @param axis is the axis to check for.
	 * @return true if the axis is enabled and processing of the message
	 * can in fact continue.
	 */
	inline bool ENABLED (int axis);

	virtual void Body(void);

	/// helper functions
	int _writeWord16 (int msg, int axis, short s);
	int _writeWord16Ex (int msg, int axis, short s1, short s2);
	int _readWord16 (int msg, int axis, short& value);
	int _readDWord (int msg, int axis, int& value);
	int _writeDWord (int msg, int axis, int value);
	int _writeNone (int msg, int axis);

	/// internal stuff.
	double *_ref_speeds;
	double *_ref_accs;
	double *_ref_positions;

	enum { MAX_SHORT = 32767, MIN_SHORT = -32768, MAX_INT = 0x7fffffff, MIN_INT = 0x80000000 };

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