
// SerialProtocol.cpp: implementation of the SerialProtocol class.
//
//////////////////////////////////////////////////////////////////////

#include "YARPSerialProtocol.h"

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
	char sw=1;
	int ret=YARP_FAIL;

	while(sw)
	{
		// printf("Receiving:\n");
		if(_readbytes(&byte,1)==1)
		{
		  // printf("%x\n", byte);
		  if(((byte>>4)&0x0F)==_count)
		  {
		      if(_count&0x01)
				  _dataIn[_count>>1]=_half |(byte&0x0F);
			  else
				  _half=(byte<<4)&0xF0;
		  
			  _count++;
			  if (_count==__spPacketSize)
			  {
				  _count=0;
				  ret=YARP_OK;
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
			ret=YARP_FAIL;
		}
	}

	return ret;
}

int SerialProtocol::writeFormat2bytes()
{
	unsigned char byte;  
	int countb;

	for(countb=0;countb<__spPacketSize;countb++)
	{
	    if(countb&0x01)
			byte= _rawData2Send[countb>>1]&0x0F;
		else
			byte= (_rawData2Send[countb>>1]>>4)&0x0F;
		byte= byte | ((countb<<4)&0xF0);
	    _formatedData2Send[countb]=byte;
	}

	// printf("Sending:\n");
	/*for(int o = 0; o<__spPacketSize; o++)
	{
		printf("%x\n", _formatedData2Send[o]);
	}*/
	if (_writebytes(_formatedData2Send, __spPacketSize) == __spPacketSize)
		return YARP_OK;
	else
		return YARP_FAIL;
}

int SerialProtocol::close()
{
	return YARP_OK;
}

int SerialProtocol::open(const char* PortName)
{
	struct termios p_termios;
	speed_t speed;

	_fd = ::open(PortName, O_RDWR | O_NDELAY);
	
    //Program the serial port
	if( tcgetattr(_fd,&p_termios) )
	{
		ACE_OS::printf("Problem opening serial port\n");
		return YARP_FAIL;
	}
	ACE_OS::printf("Serial Port Open. ID= %d\n",_fd);

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

	return YARP_OK;
}
