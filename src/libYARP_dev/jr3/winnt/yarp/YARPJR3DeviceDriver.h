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
/// $Id: YARPJR3DeviceDriver.h,v 1.1 2004-07-13 13:21:07 babybot Exp $
///
///

#ifndef __YARP_JR3_DEVICE_DRIVER__
#define __YARP_JR3_DEVICE_DRIVER__


#include <yarp/YARPConfig.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPSemaphore.h>

struct JR3OpenParameters
{
	JR3OpenParameters()
	{
		_vendorID = 0x1762;
		_deviceID = 0x1111;
		_nump = 1;
	}

	int _vendorID;
	int _deviceID;
	int _nump;
};

// command list
enum JR3Cmd
{
	CMDJR3ReadData = 0,
	CMDJR3ResetOffsets = 1,
	CMDJR3SetActualFullScales = 2,
	CMDJR3SetMinFullScales = 3,
	CMDJR3SetMaxFullScales = 4,
	JR3NCmds = 5, // required! tells the total number of commands
};

class YARPJR3DeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPJR3DeviceDriver>
{
public:
	YARPJR3DeviceDriver() : YARPDeviceDriver<YARPNullSemaphore, YARPJR3DeviceDriver>(JR3NCmds)
	{
		m_cmds[CMDJR3ReadData] = &YARPJR3DeviceDriver::readData;
		m_cmds[CMDJR3ResetOffsets] = &YARPJR3DeviceDriver::resetOffsets;
		m_cmds[CMDJR3SetActualFullScales] = &YARPJR3DeviceDriver::setActualFullScales;
		m_cmds[CMDJR3SetMaxFullScales] = &YARPJR3DeviceDriver::setMaxFullScales;
		m_cmds[CMDJR3SetMinFullScales] =  &YARPJR3DeviceDriver::setMinFullScales;
	}

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

private:
	int resetOffsets(void *);
	int setActualFullScales(void *f);
	int setMinFullScales(void *);
	int setMaxFullScales(void *);
	int readData(void *f);

private:
	int p_num;
};

#endif