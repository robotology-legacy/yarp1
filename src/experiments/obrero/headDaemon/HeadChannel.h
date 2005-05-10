// HeadChannel.h: interface for the HeadChannel class.
//
//////////////////////////////////////////////////////////////////////



#if !defined(HEADCHANNEL)
#define HEADCHANNEL


#include "SPICommBoard.h"

class HeadChannel  
{
public:
	int update_buffer();
	
	SPICommBoard * spi_pointer;

	int send_setpoint_pot(unsigned char motornumber, int setpoint);

	char SizeBufferTX;
	char SizeBufferRX;

	unsigned char address;

	unsigned char * BufferTX;
	unsigned char * BufferRX;
	
	
	HeadChannel(unsigned char add, SPICommBoard * pointer);
	virtual ~HeadChannel();

};

#endif // HEADCHANNEL
