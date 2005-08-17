// HeadChannel.cpp: implementation of the HeadChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "HeadChannel.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

HeadChannel::HeadChannel(unsigned char add, const char *const parPortAddr)
{
  address=add;
  SizeBufferRX=8;
  SizeBufferTX=4;

  BufferRX=new unsigned char[SizeBufferRX];
  BufferTX=new unsigned char[SizeBufferTX];

  parPort.openport(parPortAddr);
  
  update_buffer();

  ref[0]=BufferRX[2];//x
  ref[1]=BufferRX[0];//y
}

HeadChannel::~HeadChannel()
{
  delete [] BufferRX;
  delete [] BufferTX;
}

int HeadChannel::send_setpoint_pot(unsigned char motornumber, int setpoint)
{
  // new code for debug purpose
  //  parPort.ReadBuffer(address, BufferRX, SizeBufferRX);
  //  fprintf(stderr, "%.4d\t%.4d", BufferRX[2], BufferRX[0]);

  
  // debug
  //  for(int i = 0; i<8; i++)
  //printf("%2x", BufferRX[i]);
  //  printf("\n");

  //range of pots 0xffc0 0x0000
  // 10 higher bits

  int set=setpoint;
  unsigned char Hbyte, Lbyte;

  BufferTX[0]=motornumber;
  BufferTX[1]=0x00;
  BufferTX[2]=(unsigned char)setpoint; 
  BufferTX[3]=0x00;

  parPort.WriteBuffer(address,BufferTX,SizeBufferTX);

  //  fprintf(stderr, "%.4d\t%.4d", setpoint);

  //  motornumber, setpoint);
  ref[motornumber]=setpoint;
  return 0;
}

int HeadChannel::update_buffer()
{
  parPort.ReadBuffer(address, BufferRX, SizeBufferRX);
  // debug
  //  for(int i = 0; i<8; i++)
  //printf("%2x", BufferRX[i]);
  //  printf("\n");

  return 0;
}

void HeadChannel::getRefPos(int &x, int &y)
{
  x=ref[1];
  y=ref[0];
}
