// HeadChannel.cpp: implementation of the HeadChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadChannel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeadChannel::HeadChannel(unsigned char add, SPICommBoard * pointer)
{

		address=add;
		SizeBufferRX=8;
		SizeBufferTX=4;

		BufferRX=new unsigned char[SizeBufferRX];
		BufferTX=new unsigned char[SizeBufferTX];

		spi_pointer=pointer;
}

HeadChannel::~HeadChannel()
{
	delete [] BufferRX;
	delete [] BufferTX;
}

int HeadChannel::send_setpoint_pot(unsigned char motornumber, int setpoint)
{
	//range of pots 0xffc0 0x0000
	// 10 higher bits

	int	set=setpoint;
	unsigned char Hbyte, Lbyte;

/*
	if(sett>0x3FF)
	{
		set=0x3FF;
	}

	if(set<0)
	{
		set=0;
	}

*/


	



	BufferTX[0]=motornumber;
	BufferTX[1]=0x00;
	BufferTX[2]=(unsigned char)setpoint; 
	BufferTX[3]=0x00;


	spi_pointer->WriteBuffer(address,BufferTX,SizeBufferTX);

	return(0);
}

int HeadChannel::update_buffer()
{
	spi_pointer->ReadBuffer(address, BufferRX, SizeBufferRX);
	return(0);
}
