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
/// $Id: YARPSciDeviceDriver.cpp,v 1.4 2005-02-24 22:12:08 natta Exp $
///
///

/// general purpose stuff.
#include <yarp/YARPConfig.h>
#include <yarp/YARPControlBoardUtils.h>

/// specific to this device driver.
#include "YARPSciDeviceDriver.h"
#include "sci_messages.h"
const int MAX_ADC = 15;

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
	SingleAxisParameters *tmp = (SingleAxisParameters *) cmd;
	const int axis = tmp->axis;
	int value;

	ACE_ASSERT (axis >= 0 && axis <= (MAX_ADC));

	if (_readWord(SCI_READ_POTENTIOMETERS, axis, value) == YARP_OK)
	{
		*((double *)tmp->parameters) = (double) (value);
		return YARP_OK;
	}
	else
	{
		*((double *)tmp->parameters) = 0.0;
		return YARP_FAIL;
	}
}

int YARPSciDeviceDriver::_readWord(int msg, int joint, int &value)
{
	_serialPort._rawData2Send[0] = msg;
	_serialPort._rawData2Send[1] = joint;

	_serialPort.writeFormat2bytes();

	_serialPort.readFormat2bytes();

	value = _serialPort._dataIn[0]+_serialPort._dataIn[1]*256;
}

// SerialProtocol.cpp: implementation of the SerialProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "SerialProtocol.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SerialProtocol::SerialProtocol()
{
	_count=0;
}

SerialProtocol::~SerialProtocol()
{
	close();
}

int SerialProtocol::readFormat2bytes()
{
	unsigned char byte;
	char sw=1,r=0;
	
	while(sw)
	{
		if(_readbytes(&byte,1)==1)
		{
		  //printf("Count= %d Data %X\n",count,byte);
		  if(((byte>>4)&0x0F)==_count)
		  {
		      if(_count&0x01)
				  _dataIn[_count>>1]=_half |(byte&0x0F);
			  else
				  _half=(byte<<4)&0xF0;
		  
			  _count++;
			  if (_count==16)
			  {
				_count=0;
				r=1;
				sw=0;
			  }
		  }
		  else
		  {
			  _count=0;
			  if (!(byte&0xF0))
			  {
				  _count=1;
				  _half=(byte<<4)&0xF0;
			  }
		  }
		}
		else
		{
			sw=0;
			r=0;
		}
	}

	return((int)r);
}

int SerialProtocol::writeFormat2bytes()
{
	unsigned char byte;  
	int countb;

	for(countb=0;countb<16;countb++)
	{
	    if(countb&0x01)
			byte= _rawData2Send[countb>>1]&0x0F;
		else
			byte= (_rawData2Send[countb>>1]>>4)&0x0F;
		byte= byte | ((countb<<4)&0xF0);
	    _formatedData2Send[countb]=byte;
	}

	_writebytes(_formatedData2Send,16);
	return(1);
}

int SerialProtocol::close()
{
	return 1;
}

int SerialProtocol::open(const char* PortName)
{
	struct termios p_termios;
	speed_t speed;

	_fd = ::open(PortName, O_RDWR | O_NDELAY);
	printf("Serial Port Open. ID= %d\n",_fd);

  //Program the serial port
	if( tcgetattr(_fd,&p_termios) )
	{
		printf("Serial Port no working\n");
		return(-1);
	}
	//RAW mode
	//revisar
	p_termios.c_cc[VMIN]=0;
	p_termios.c_cc[VTIME]=0;
	p_termios.c_lflag &= ~(ECHO|ICANON|ISIG|ECHOE|ECHOK|ECHONL);
	p_termios.c_oflag &= ~(OPOST);

	//Check Termios.h to find out the constant Bxxxx that correspond to 
	// your desired speed.

	speed=B115200;
	
	cfsetispeed(&p_termios,speed);
	cfsetospeed(&p_termios,speed);
	cfmakeraw(&p_termios);

	tcsetattr(_fd,TCSADRAIN,&p_termios);

	return (1);
}

int SerialProtocol::_readbytes(unsigned char * sBuffer, char num)
{
	int n;
	n = read(_fd,  sBuffer, num );
	//if(n==1) printf("[%x],(%d)",sBuffer[0],n);
	if (n<0) n=0;

	return(n);
}

int SerialProtocol::_writebytes(unsigned char *strTB, char cSize)
{
	int	iNbytesWritten;

	iNbytesWritten=write(_fd,strTB,cSize);
	//printf("writing: %d, %s\n",iNbytesWritten,strTB);
	return(iNbytesWritten);
}





