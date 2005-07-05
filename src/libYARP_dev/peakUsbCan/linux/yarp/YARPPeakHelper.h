// YARPPeakHelper.h: interface for the PeakHelper class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#if !defined(__YARPPEAKHELPERH__)
#define __YARPPEAKHELPERH__

struct PEAK_CAN_MESSAGE
{
  int _id;
  char _type;
  char _len;
  char _data[8];
};


//these are taken from libpcan.h
#define PEAK_CAN_BAUD_1M     0x0014  //   1 MBit/s
#define PEAK_CAN_BAUD_500K   0x001C  // 500 kBit/s
#define PEAK_CAN_BAUD_250K   0x011C  // 250 kBit/s
#define PEAK_CAN_BAUD_125K   0x031C  // 125 kBit/s
#define PEAK_CAN_BAUD_100K   0x432F  // 100 kBit/s
#define PEAK_CAN_BAUD_50K    0x472F  //  50 kBit/s
#define PEAK_CAN_BAUD_20K    0x532F  //  20 kBit/s
#define PEAK_CAN_BAUD_10K    0x672F  //  10 kBit/s  
#define PEAK_CAN_BAUD_5K     0x7F7F  //   5 kBit/s  

#define PEAK_CAN_INIT_TYPE_EX                0x01    //Extended Frame
#define PEAK_CAN_INIT_TYPE_ST                0x00    //Standart Frame

/**
 * \file YARPSciDeviceDriver.h 
 * class for interfacing with the value can device driver.
 */

const char __defaultPortName[] = "/dev/pcan32";
struct PeakOpenParameters
{
  /**
   * Constructor.
   */
  PeakOpenParameters (const char *portName=NULL)
  {
    if (portName!=NULL)
      strcpy(_portname, portName);
    else
      strcpy(_portname, __defaultPortName);

    _wBTR0BTR1=PEAK_CAN_BAUD_1M;
    _frameType=PEAK_CAN_INIT_TYPE_ST;
  }
	
  char _portname[80];
  int _njoints;
  short _wBTR0BTR1;   // can bus BTR0/BTR1 registers; together they define the bit rate
  int _frameType;     // standard or extended frames
  int _id;            // CAN ID (sender/receiver)
};

class PeakHelper  
{
public:
	PeakHelper();
	virtual ~PeakHelper();

	int write(const unsigned char *data);
	int read(unsigned char *data);
	int read();

	int close();
	int open(PeakOpenParameters *p);

private:
	void *_inBuf;
	void *_outBuf;
	void *_handle;
};

#endif // h
