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
//
// Interface for the NIDAQ device driver.
//
// Feb 2003 -- by nat
//
// win32: link nidaq32.lib
// $Id: YARPNIDAQDeviceDriver.h,v 1.1 2004-07-13 13:21:10 babybot Exp $

#ifndef __YARPNIDAQDeviceDriver__
#define __YARPNIDAQDeviceDriver__

#include <yarp/YARPConfig.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPDeviceDriver.h>
#include <yarp/YARPADCUtils.h>

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

class YARPNIDAQDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPNIDAQDeviceDriver> 
{
public:
	YARPNIDAQDeviceDriver() : YARPDeviceDriver<YARPNullSemaphore, YARPNIDAQDeviceDriver>(ADCCmds)
	{
		m_cmds[CMDAIConfigure] = YARPNIDAQDeviceDriver::ai_configure;
		m_cmds[CMDScanSetup] = YARPNIDAQDeviceDriver::scan_setup;
		m_cmds[CMDAIVReadScan] = YARPNIDAQDeviceDriver::ai_vread_scan;
		m_cmds[CMDAIReadScan] = YARPNIDAQDeviceDriver::ai_read_scan;
	}

	// overload Open, Close
	virtual int open(void *d);
	virtual int close(void);

private:
	int ai_configure(void *p);
	int scan_setup(void *p);
	int ai_vread_scan(void *p);
	int ai_read_scan(void *p);
};

#endif //.h