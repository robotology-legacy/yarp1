// by nat June 2003

#ifndef __YARPJR3Adapterh__
#define __YARPJR3Adapterh__


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <math.h>
#include <YARPConfigFile.h>

#include <YARPJR3DeviceDriver.h>

class YARPJR3Params
{
public:
	YARPJR3Params()
	{
		_theta = 0.0;
		_max = 1.0;

		_R[0][0] = 1.0;
		_R[0][1] = 0.0;
		_R[0][2] = 0.0;

		_R[1][0] = 0.0;
		_R[1][1] = 1.0;
		_R[1][2] = 0.0;
		
		_R[2][0] = 0.0;
		_R[2][1] = 0.0;
		_R[2][2] = 1.0;
	}

	int load(const std::string &path, const std::string &init_file)
	{
		YARPConfigFile cfgFile;
		// set path and filename
		cfgFile.set(path.c_str(),init_file.c_str());
		
		// get number of joints
		if (cfgFile.get("[GENERAL]", "Max", &_max, 1) == YARP_FAIL)
			return YARP_FAIL;

		if (cfgFile.get("[GENERAL]", "Theta", &_theta, 1) == YARP_FAIL)
			return YARP_FAIL;

		_R[0][0] = cos(_theta);
		_R[0][1] = sin(_theta);
		_R[0][2] = 0.0;

		_R[1][0] = -sin(_theta);
		_R[1][1] = cos(_theta);
		_R[1][2] = 0.0;
		
		_R[2][0] = 0.0;
		_R[2][1] = 0.0;
		_R[2][2] = 1.0;

		return YARP_OK;
	}
	
	double _theta;
	double _max;
	double _R[3][3];
};	//namespace


class YARPJR3Adapter: public YARPJR3DeviceDriver
{
public:
	int initialize ()
	{
		JR3OpenParameters openP;
		/// need additional initialization, put it here.
		open ((void *)&openP);

		IOCtl(CMDJR3ResetOffsets, NULL);
		IOCtl(CMDJR3SetMinFullScales, NULL);

		return YARP_OK;
	}

	int uninitialize (void)
	{
		/// need additional termination stuff, here's the place for it.
		return close ();
	}
};

#endif