// July 2003 -- by nat

// $Id: YARPTouchBoard.h,v 1.1 2003-07-11 14:12:03 babybot Exp $

#ifndef __YARPTOUCH_BOARD__
#define __YARPTOUCH_BOARD__

#include <ace/log_msg.h>
#include <YARPAndroidDeviceDriver.h>
#include <YARPTouchBoardParameters.h>
#include <string>

#define YARP_TOUCH_BOARD_VERBOSE

#ifdef YARP_TOUCH_BOARD_VERBOSE
#define YARP_TOUCH_BOARD_VERBOSE_DEBUG(string) YARP_DEBUG("YARP_TOUCH_BOARD_VERBOSE_DEBUG:", string)
#else  YARP_TOUCH_BOARD_VERBOSE_DEBUG(string) YARP_NULL_DEBUG
#endif

class YARPTouchBoard : public YARPAndroidDeviceDriver
{
public:
	YARPTouchBoard()
	{
		_initialized = false;
	}
	
	~YARPTouchBoard()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(const std::string &filename)
	{
		_parameters.load("", filename);
		
		AndroidOpenParameters opar;
		opar.pport = _parameters._port;
		open(&opar);
	
		AndroidScanSetupParameters spar;
		spar.nBoard = _parameters._board;
		IOCtl(CMDScanSetup, &spar);
		
		_nSensors = _parameters._nS;

		_initialized = true;
		return YARP_OK;
	}

	int read(double *out)
	{
		IOCtl(CMDAIReadScan, _temp_char);
	
		_convertOutput(_temp_char, out);

		return YARP_OK;
	}

	int _convertOutput(const unsigned char *in, double *out)
	{
		for(int i = 0; i < _nSensors; i++)
			out[i] = 255-in[_parameters._lut[i]];
		return YARP_OK;
	}

	int uninitialize()
	{
		close();

		_initialized = false;
		return YARP_OK;
	}
	
private:

	bool _initialized;
	unsigned char _temp_char[__AndroidNInput];
	int _nSensors;
	YARPTouchBoardParameters _parameters;
};

#endif