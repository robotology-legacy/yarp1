// YARPSerialProtocol.h: interface for the SerialProtocol class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__YARPSERIALPROTOCOLH__)
#define __YARPSERIALPROTOCOLH__

// check implementation file before changing these constants
const int __spBufferSize = 8;
const int __spPacketSize = 2*__spBufferSize;

class SerialProtocol  
{
public:
	
	SerialProtocol();
	virtual ~SerialProtocol();

	int writeFormat2bytes();
	int readFormat2bytes();

	int close();
	int open(const char* PortName);

	unsigned char _dataIn[__spBufferSize];
	unsigned char _formatedData2Send[__spPacketSize];
	unsigned char _rawData2Send[__spBufferSize];

private:
	inline int _readbytes(unsigned char * sBuffer, char num)
	{
		int n;
		n = read(_fd,  sBuffer, num );
		if (n<0) n=0;
			return n;
	}

	inline int _writebytes(unsigned char *strTB, char cSize)
	{  return write(_fd,strTB,cSize); }

	int _fd;
	int _count;
	unsigned char _half;
};

#endif // SERIALPROTOCOL
