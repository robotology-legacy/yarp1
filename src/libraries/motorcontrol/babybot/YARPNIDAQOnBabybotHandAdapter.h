// originally feb 2003 -- by nat
// adapted to yarp June 2003 -- by nat

// $Id: YARPNIDAQOnBabybotHandAdapter.h,v 1.1 2003-06-30 21:09:20 babybot Exp $

#ifndef __YARPNIDAQONBABYBOTHANDADAPTERH__
#define __YARPNIDAQONBABYBOTHANDADAPTERH__

#include <ace/log_msg.h>
#include <YARPNIDAQDeviceDriver.h>
#include <string>

#include <YARPConfigFile.h>

#include "YARPBabybotHandParameters.h"

#define YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_VERBOSE

#ifdef YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_VERBOSE
#define YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_DEBUG(string) YARP_DEBUG("YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_DEBUG:", string)
#else  YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_DEBUG(string) YARP_NULL_DEBUG
#endif

class YARPNIDAQOnBabybotHandAdapter : public YARPNIDAQDeviceDriver
{
public:
	YARPNIDAQOnBabybotHandAdapter()
	{
		_initialized = false;
	}
	
	~YARPNIDAQOnBabybotHandAdapter()
	{
		if (_initialized)
			uninitialize();
	}

	int initialize(YARPBabybotHandParameters *par)
	{
		int rc;
		_parameters = par;
		// open nidaq
		NIDAQOpenParameters niopen;
		niopen.device_id = par->_nidaq_id;
		rc = open(&niopen);

		if (rc != 0)
		{
			YARP_NIDAQ_ON_BABYBOT_HAND_ADAPTER_DEBUG(("Error: cannot initialize NIDAQ control board !\n"));
			return rc;
		}

		NIDAQAIConfigureParameters ni_ai_p;
		ni_ai_p.chan = -1;
		ni_ai_p.inputMode = par->_nidaq_input_mode;
		ni_ai_p.polarity = par->_nidaq_input_polarity;
		rc = IOCtl(CMDAIConfigure, &ni_ai_p);
	
		NIDAQScanSetupParameters ni_scan_p;
		ni_scan_p.nCh = par->_nidaq_ch;
		ni_scan_p.ch_sequence = par->_nidaq_ch_seq;
		ni_scan_p.ch_gains = par->_nidaq_gains;
		rc = IOCtl(CMDScanSetup, &ni_scan_p);

		_initialized = true;
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

	YARPBabybotHandParameters *_parameters;
};

#endif