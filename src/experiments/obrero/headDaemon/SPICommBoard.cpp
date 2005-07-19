// SPICommBoard.cpp: implementation of the SPICommBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "SPICommBoard.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SPICommBoard::SPICommBoard()
{

}

SPICommBoard::~SPICommBoard()
{

}

int SPICommBoard::ReadStatus(unsigned char address, unsigned char *data)
{

  // Read status
  writeaddress(address+1);
  writedata(0);//dummy
  writeaddress(address+0);
  readdata(data);

  return(1);
}

int SPICommBoard::WriteBuffer(unsigned char address, unsigned char *pointer, char size)
{
  unsigned char status;
  int ticks=0;
  int i;

  while(1)
    {
      // Read status
      writeaddress(address+1);
      writedata(0);//dummy
      writeaddress(address+0);
      readdata(&status);
				
      // check if busy sending data
      if((status & 0x01)==0)
	{
					
	  // Reset Writing buffer
	  writeaddress(address+0);
	  writedata(0x02);
					
	  //write data 4 bytes
	  for(i=0;i<size;i++)
	    {
	      writeaddress(address+2);
	      writedata(pointer[i]);
	    }
					
	  ticks=0;
	  return(1);

					
	}
      else
	{
	  ticks++;
	  if(ticks>10)
	    return(0);
	}
    }

  return(1);
}

int SPICommBoard::ReadBuffer(unsigned char address, unsigned char* pointer, char size)
{
  int i;

  // Reset Reading buffer
  writeaddress(address+0);
  writedata(0x01);
		
  //Read data 8 bytes
  for(i=0;i<size;i++)
    {
      writeaddress(address+0);
      readdata(&(pointer[i]));
    }

}
