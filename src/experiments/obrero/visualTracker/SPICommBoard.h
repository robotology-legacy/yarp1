// SPICommBoard.h: interface for the SPICommBoard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined SPICOMMBOARD
#define SPICOMMBOARD


#include "ParallelPort.h"

class SPICommBoard : public ParallelPort  
{
public:
	int ReadBuffer(unsigned char address, unsigned char * pointer, char size);
	int WriteBuffer(unsigned char address, unsigned char * pointer, char size);
	int ReadStatus(unsigned char address, unsigned char *data);
	SPICommBoard();
	virtual ~SPICommBoard();

};

#endif // SPICOMMBOARD
