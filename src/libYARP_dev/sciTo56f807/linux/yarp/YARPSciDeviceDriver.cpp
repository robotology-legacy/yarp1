/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2003           ///
///                                                                   ///
///                    #nat#                         ///
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
/// $Id: YARPSciDeviceDriver.cpp,v 1.6 2005-02-25 17:04:34 natta Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>

/// specific to this device driver.
#include "YARPSciDeviceDriver.h"
#include "sci_messages.h"
const int MAX_ADC = 15;
#include <yarp/YARPTime.h>

/// get the message types from the DSP code.
// #include "../56f807/cotroller_dc/Code/controller.h"


YARPSciDeviceDriver::YARPSciDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPSciDeviceDriver>(CBNCmds), _mutex(1)
{
	m_cmds[CMDGetPosition] = &YARPSciDeviceDriver::getPosition;
}

YARPSciDeviceDriver::~YARPSciDeviceDriver ()
{
	
}

int YARPSciDeviceDriver::open (void *res)
{
	_mutex.Wait();

	SciOpenParameters *par = (SciOpenParameters *)(res);
	int ret = _serialPort.open(par->_portname);

	_mutex.Post();

	return ret;
}

int YARPSciDeviceDriver::close (void)
{
	_mutex.Wait(); 

	int ret = _serialPort.close();
	
	_mutex.Post();

	return ret;
}

int YARPSciDeviceDriver::getPosition(void *cmd)
{
	int ret;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	int value;

	ACE_ASSERT (axis >= 0 && axis <= (MAX_ADC));

	ret = _readWord(SCI_READ_POTENTIOMETERS, axis, value);
	// if ret != YARP_OK value == 0, so no need to check ret
	*((double *)tmp->parameters) = (double) (value);
	
	return ret;
}

int YARPSciDeviceDriver::_readWord(int msg, int joint, int &value)
{
	int ret;

	_serialPort._rawData2Send[0] = msg;
	_serialPort._rawData2Send[1] = joint;

	ret = _serialPort.writeFormat2bytes();

	if (ret != YARP_OK)
	{
		value = -1;
		return ret;
	}

	YARPTime::DelayInSeconds(0.005);

	ret = _serialPort.readFormat2bytes();

	if (ret != YARP_OK)
	{
		value = 0;
		return YARP_FAIL;
	}
	else
	{
		value = _serialPort._dataIn[0]+_serialPort._dataIn[1]*256;
		return YARP_OK;
	}
}