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
/// $Id: YARPSciDeviceDriver.cpp,v 1.8 2005-03-03 23:07:33 natta Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>

/// specific to this device driver.
#include "YARPSciDeviceDriver.h"
#include "sci_messages.h"
const int MAX_ADC = 15;
const int __nj = 6;
#include <yarp/YARPTime.h>

/// get the message types from the DSP code.
// #include "../56f807/cotroller_dc/Code/controller.h"


YARPSciDeviceDriver::YARPSciDeviceDriver(void) 
	: YARPDeviceDriver<YARPNullSemaphore, YARPSciDeviceDriver>(CBNCmds), _mutex(1)
{
		
	m_cmds[CMDGetPosition] = &YARPSciDeviceDriver::getPosition;
	m_cmds[CMDGetPositions] = &YARPSciDeviceDriver::getPositions;
	m_cmds[CMDSetPositionControlMode] = &YARPSciDeviceDriver::setPositionMode;
	m_cmds[CMDSetPosition] = &YARPSciDeviceDriver::setPosition;
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

int YARPSciDeviceDriver::getPositions(void *cmd)
{
	int ret;
	ACE_ASSERT (cmd!=NULL);
	double *tmp = (double *) cmd;
	ret = _readVector(SCI_READ_POSITIONS_0TO3, tmp, 4);

//	YARPTime::DelayInSeconds(0.3);
	
	ret = _readVector(SCI_READ_POSITIONS_4TO5, tmp+4, 2);
	return ret;
}

int YARPSciDeviceDriver::setPositionMode(void *cmd)
{
	int ret;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	int value;
	
	ret = _writeWord(SCI_POSITION_CONTROL_MODE, axis);
	return ret;
}

int YARPSciDeviceDriver::setForceMode(void *cmd)
{
	int ret;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	int value;
	
	ret = _writeWord(SCI_FORCE_CONTROL_MODE, axis);
	return ret;
}

int YARPSciDeviceDriver::setPosition(void *cmd)
{
	int ret;
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	double value = *((double *) tmp->parameters);

	ret = _writeWord(SCI_SET_POSITION, axis, (int)(value));
	return ret;
}

int YARPSciDeviceDriver::_readVector(char msg, double *v, int n)
{
	int ret;

	_serialPort._rawData2Send[0] = msg;

	ret = _serialPort.writeFormat2bytes();

	if (ret != YARP_OK)
	{
		for (int i = 0; i <n; i++)
			v[i] = -1.0;
		return ret;
	}

	ret = _serialPort.readFormat2bytes();

	if (ret != YARP_OK)
	{
		for (int i = 0; i <n; i++)
			v[i] = -1.0;
		return YARP_FAIL;
	}
	else
	{
		int jj = 0;
		for (int i = 0; i <n; i++)
		{
			v[i] = _serialPort._dataIn[jj]+_serialPort._dataIn[jj+1]*256;
			jj+=2;
		}
		return YARP_OK;
	}
}

int YARPSciDeviceDriver::_readWord(char msg, char joint, int &value)
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

int YARPSciDeviceDriver::_writeWord(char msg, char joint)
{
	int ret;

	_serialPort._rawData2Send[0] = msg;
	_serialPort._rawData2Send[1] = joint;

	ret = _serialPort.writeFormat2bytes();

	return ret;
}

int YARPSciDeviceDriver::_writeWord(char msg, char joint, int value)
{
	int ret;

	_serialPort._rawData2Send[0] = msg;
	_serialPort._rawData2Send[1] = joint;
	_serialPort._rawData2Send[2] = value;
	_serialPort._rawData2Send[3] = (value>>8);

	ret = _serialPort.writeFormat2bytes();

	return ret;
}

void YARPSciDeviceDriver::readDebugger()
{
	char byte;
	char byte1, byte2, byte3, byte4;
	char tmp[255];	//later, change this or control overflow
	char *pTmp;
	bool done = false;
	int index = 0;

	tmp[0]='\0';
	pTmp = tmp;

	while(!done)
	{
		byte = _serialPort.readRaw();

		if(byte=='\r')
		{
			*pTmp = '\r';
			pTmp++;
			done = true;
		}
		else if (byte == '\0')
		{
			done = true;
		}
		else if (byte == '%')
		{
			// manage symbol
			byte = _serialPort.readRaw();
			if(byte=='x')
			{
				// we have a byte
				sprintf(pTmp, "0x");
				pTmp+=2;
				byte = _serialPort.readRaw();
				sprintf(pTmp,"%x",byte);
				pTmp+=2;
			}
			else if(byte=='d')
			{
				// we have a 16 word
				sprintf(pTmp, "0x");
				pTmp+=2;
				byte1 = _serialPort.readRaw();
				byte2 = _serialPort.readRaw();
				sprintf(pTmp,"%x%x", byte2,byte1);
				pTmp+=4;
			}
			else if(byte=='l')
			{
				// we have a 32 word
				sprintf(pTmp, "0x");
				pTmp+=2;
				
				byte1 = _serialPort.readRaw();
				byte2 = _serialPort.readRaw();
				byte3 = _serialPort.readRaw();
				byte4 = _serialPort.readRaw();
				sprintf(pTmp,"%x%x%x%x", byte4,byte3,byte2,byte1);
				pTmp+=8;
			}
			else
			{
				sprintf(pTmp, "_ERROR_");
				pTmp+=7;
			}
			done = true;
		}
		else
		{
			*pTmp=byte;
			pTmp++;
		}
	}

	// close string
	*pTmp = '\0';
	ACE_OS::printf("%s", tmp);
}
	
