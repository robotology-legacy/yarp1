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
	
  int send_setpoint_pot(unsigned char motornumber, int setpoint);
  void getRefPos(int &x, int &y);

  char SizeBufferTX;
  char SizeBufferRX;

  unsigned char address;

  unsigned char * BufferTX;
  unsigned char * BufferRX;
	
	
  HeadChannel(unsigned char add, const char *const parPortAddr);
  ~HeadChannel();

 private:
  int ref[2];
  SPICommBoard parPort;

};

#endif // HEADCHANNEL
