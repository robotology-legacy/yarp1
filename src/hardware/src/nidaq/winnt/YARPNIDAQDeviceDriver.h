//
//
// Interface for the NIDAQ device driver.
//
// Feb 2003 -- by nat
//
// win32: link nidaq32.lib

#ifndef __YARPNIDAQDeviceDriver__
#define __YARPNIDAQDeviceDriver__

#include "YARPDeviceDriver.h"
#include "YARPSemaphore.h"

#ifndef _NIDAQ_Header_
typedef char           i8;
typedef unsigned char  u8;
typedef short          i16;
typedef unsigned short u16;
typedef long           i32;
typedef unsigned long  u32;
typedef float          f32;
typedef double         f64;
#endif

// command list
enum NIDAQCmd
{
	CMDAIConfigure = 0,
	CMDScanSetup = 1,
	CMDAIVReadScan = 2,
	CMDAIReadScan = 3,
	NIDAQNCmds = 4, // required! tells the total number of commands
};

struct NIDAQOpenParameters
{
	int device_id;
};

struct NIDAQAIConfigureParameters 
{
	NIDAQAIConfigureParameters()
	{
		chan = -1;			// affect all channels
		inputMode = 1;		// Referenced Single-Ended
		inputRange = 10;	// input range, it is ignored
		polarity = 1;		// unipolar
		driveAIS = 0;		// ignored
	};

	i16 chan;
	i16 inputMode;
	i16 inputRange;
	i16 polarity;
	i16 driveAIS;
};

struct NIDAQScanSetupParameters
{
	NIDAQScanSetupParameters()
	{
		nCh = 0;
		ch_sequence = 0;
		ch_gains = 0;
	};

	i16	nCh;				// # of channels
	i16 *ch_sequence;		// channel sequence
	i16	*ch_gains;			// channel gains

};

class YARPNIDAQDeviceDriver : public YARPDeviceDriver<YARPNULLSempahore, YARPNIDAQDeviceDriver> 
{
public:
	YARPNIDAQDeviceDriver() : YARPDeviceDriver<YARPNULLSempahore, YARPNIDAQDeviceDriver>(NIDAQNCmds)
	{
		m_cmds[CMDAIConfigure] = YARPNIDAQDeviceDriver::ai_configure;
		m_cmds[CMDScanSetup] = YARPNIDAQDeviceDriver::scan_setup;
		m_cmds[CMDAIVReadScan] = YARPNIDAQDeviceDriver::ai_vread_scan;
		m_cmds[CMDAIReadScan] = YARPNIDAQDeviceDriver::ai_read_scan;
	}

	// overload Open, Close
	virtual int open(void *d);
	virtual int close(void);

	int ai_configure(void *p);
	int scan_setup(void *p);
	int ai_vread_scan(void *p);
	int ai_read_scan(void *p);

protected:
};

#endif //.h