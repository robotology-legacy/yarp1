#ifndef __YARP_ADC_UTILS__
#define __YARP_ADC_UTILS__

// command list
enum ADCCmd
{
	CMDAIConfigure = 0,
	CMDScanSetup = 1,
	CMDAIVReadScan = 2,
	CMDAIReadScan = 3,
	CMDAIReadChannel = 4,
	ADCCmds = 5, // required! tells the total number of commands
};

#endif