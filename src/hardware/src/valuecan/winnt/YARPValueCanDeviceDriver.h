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
/// $Id: YARPValueCanDeviceDriver.h,v 1.5 2004-05-03 23:49:46 babybot Exp $
///
///

#ifndef __YARPValueCanDeviceDriverh__
#define __YARPValueCanDeviceDriverh__

#include <conf/YARPConfig.h>
#include <YARPDeviceDriver.h>
#include <YARPSemaphore.h>
#include <YARPThread.h>
#include <YARPTime.h>


/// max number of addressable cards in this implementation.
const int MAX_CARDS		= 16;

struct ValueCanOpenParameters
{
	ValueCanOpenParameters (void)
	{
		_port_number = -1;
		_arbitrationID = 0;
		memset (_destinations, 0, sizeof(unsigned char) * MAX_CARDS);
		_my_address = 0;
		_polling_interval = 10;
		_timeout = 2;
		_njoints = 0;
	}

	int _port_number;							/// which of the many CAN interfaces to connect to
	long _arbitrationID;						/// arbitration ID of the sent messages
	int _njoints;								/// number of joints (cards * 2)
	unsigned char _destinations[MAX_CARDS];		/// destination addresses
	unsigned char _my_address;					/// my address
	int _polling_interval;						/// thread polling interval [ms]
	int _timeout;								/// number of cycles before timing out
};

///
///
const short MAX_SHORT = 32767;
const short MIN_SHORT = -32768;
const int MAX_INT = 0x7fffffff;
const int MIN_INT = 0x80000000;


class YARPValueCanDeviceDriver : 
	public YARPDeviceDriver<YARPNullSemaphore, YARPValueCanDeviceDriver>, public YARPThread
{
private:
	YARPValueCanDeviceDriver(const YARPValueCanDeviceDriver&);
	void operator=(const YARPValueCanDeviceDriver&);

public:
	YARPValueCanDeviceDriver();
	virtual ~YARPValueCanDeviceDriver();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

	virtual int getPosition(void *cmd);
	virtual int getPositions(void *cmd);
	virtual int getRefPositions(void *cmd);
	virtual int setPosition(void *cmd);
	virtual int setPositions(void *cmd);
	virtual int setSpeed(void *cmd);
	virtual int setSpeeds(void *cmd);
	virtual int getRefSpeeds(void *cmd);
	virtual int setAcceleration(void *cmd);
	virtual int setAccelerations(void *cmd);
	virtual int getRefAccelerations(void *cmd);
	virtual int setOffset(void *cmd);
	virtual int setOffsets(void *cmd);
	virtual int setPid(void *cmd);
	virtual int getPid(void *cmd);
	virtual int setIntegratorLimit(void *cmd);
	virtual int setIntegratorLimits(void *cmd);
	virtual int definePosition(void *cmd);
	virtual int definePositions(void *cmd);
	virtual int enableAmp(void *cmd);
	virtual int disableAmp(void *cmd);
	virtual int controllerIdle(void *cmd);
	virtual int controllerRun(void *cmd);
	virtual int velocityMove(void *cmd);

protected:
	void *system_resources;
	YARPSemaphore _mutex;
	YARPEvent _ev;
	bool _request;
	bool _noreply;

	virtual void Body(void);

	/// helper functions
	int _writeWord16 (int msg, int axis, short s);
	int _writeWord16Ex (int msg, int axis, short s1, short s2);
	int _readWord16 (int msg, int axis, short& value);
	int _readDWord (int msg, int axis, int& value);
	int _writeDWord (int msg, int axis, int value);
	int _writeNone (int msg, int axis);

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