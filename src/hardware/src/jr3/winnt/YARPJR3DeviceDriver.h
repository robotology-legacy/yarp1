#ifndef __YARP_JR3_DEVICE_DRIVER__
#define __YARP_JR3_DEVICE_DRIVER__


#include <conf/YARPConfig.h>
#include <YARPDeviceDriver.h>
#include <YARPSemaphore.h>

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