#ifndef __ANDROID_DD__
#define __ANDROID_DD__

#include <YARPDeviceDriver.h>
#include <YARPSemaphore.h>
#include "YARPADCUtils.h"

#include <conio.h>

const int __AndroidNInput = 32;	// 8x4
const int __AndroidNChips = 4;
const int __AndroidNChannels = 8;
const int __trials = 1;

struct AndroidOpenParameters
{
	AndroidOpenParameters()
	{
		pport = 0x378;
	}

	unsigned short pport;
};

struct AndroidScanSetupParameters
{
	AndroidScanSetupParameters()
	{
		nBoard = 0;			// first board
	};

	int nBoard;				// board #
};

struct AndroidSingleChannel
{
	int chip;				// chip # 0,1,2,3
	int channel;
	unsigned char *data;
};

class YARPAndroidDeviceDriver : public YARPDeviceDriver<YARPNullSemaphore, YARPAndroidDeviceDriver > 
{
public:
	YARPAndroidDeviceDriver ();

	// overload open, close
	virtual int open(void *d);
	virtual int close(void);

private:
	unsigned short _pport;

	unsigned char _chipSel;
	unsigned char _boardSel;
	unsigned char _enable;
	
	void selChip(unsigned char chip, unsigned char board, unsigned char enable)
	{
		unsigned char c = 0;
		c += (chip & 0x3);				// 1st and 2nd bits
		c += ((board << 2) & 0xC);		// 3rd and 4th bits
		c += ((enable << 6) & 0x40);	// 7th bit
		int trials = __trials;
		while(trials--)
			_outp(_pport, c);
	}

	unsigned char readMSB()
	{
		int c;
		int trials = __trials;
		while(trials--)
			c = _inp(_pport+1);
		if (c&0x8)
			return 1;	// msb only
		else 
			return 0;
	}

	void outPort2(unsigned char ck, unsigned char d)
	{
		unsigned char c = 0;
		if (ck == 0)
			c += 1;
		if (d == 0)
			c += 2;
		int trials = __trials;
		while(trials--)
			_outp(_pport+2,c);
	}

	unsigned char _parOut(unsigned char ck, unsigned char d)
	{
		int trials = __trials;
		while(trials--)
			selChip(_chipSel, _boardSel, _enable);
		
		trials = __trials;
		while(trials--)
			outPort2(ck, d);
		
		return readMSB();
	}

	unsigned char readADC(unsigned char chip, unsigned char board, int channel);
	unsigned char read(unsigned char chip, unsigned char board, int channel);

	int ai_read_channel(void *d);
	int ai_read_scan(void *d);
	int scan_setup(void *b);

	unsigned char _temp_data[__AndroidNInput];
};
#endif