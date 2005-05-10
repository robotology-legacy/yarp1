// ParallelPort.h: interface for the ParallelPort class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(PARALLELPORT)
#define PARALLELPORT


#define DELAY_EPP	1e5

class ParallelPort  
{
public:
	int contolline(unsigned char mask,char value);
	int closeport();
	int readdata(unsigned char * value);
	int writedata(unsigned char value);
	int writeaddress(unsigned char value);

	int fd;
	char portn[255];

	
	int openport(char * portname);
	ParallelPort();
	virtual ~ParallelPort();

};

#endif 
