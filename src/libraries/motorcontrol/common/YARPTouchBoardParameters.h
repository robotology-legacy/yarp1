// July 2003 -- by nat

// $Id: YARPTouchBoardParameters.h,v 1.1 2003-07-11 14:12:03 babybot Exp $

#ifndef __YARPTOUCHBOARDPARAMETERS__
#define __YARPTOUCHBOARDPARAMETERS__

#include <YARPConfigFile.h>

class YARPTouchBoardParameters
{
public:
	YARPTouchBoardParameters()
	{
		_port = 0;
		_board = 0;
		_nS = 0;
		_lut = NULL;
	}

	~YARPTouchBoardParameters()
	{
		if (_lut != NULL)
			delete [] _lut;
	}

	int load(const std::string &path, const std::string &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(),init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "N", &_nS, 1) == YARP_FAIL)
			return YARP_FAIL;
		
		if (cfgFile.getHex("[GENERAL]", "Port", &_port, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GENERAL]", "Board", &_board, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (_lut != NULL)
			delete [] _lut;

		_lut = new int [_nS];

		if (cfgFile.get("[GENERAL]", "Lut", _lut, _nS) == YARP_FAIL)
			return YARP_FAIL;

		return YARP_OK;
	}

public:
	short _port;
	int _board;
	int _nS;
	int *_lut;
};

#endif