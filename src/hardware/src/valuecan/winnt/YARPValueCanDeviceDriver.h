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
/// $Id: YARPValueCanDeviceDriver.h,v 1.1 2004-04-30 13:19:37 babybot Exp $
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
	}

	int _port_number;							/// which of the many CAN interfaces to connect to
	long _arbitrationID;						/// arbitration ID of the sent messages
	unsigned char _destinations[MAX_CARDS];		/// destination addresses
	unsigned char _my_address;					/// my address
	int _polling_interval;						/// thread polling interval [ms]
	int _timeout;								/// number of cycles before timing out
};


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

	virtual int getposition(void *cmd);

protected:
	void *system_resources;
	YARPSemaphore _mutex;
	YARPEvent _ev;
	bool _request;

	virtual void Body(void);
};


#endif