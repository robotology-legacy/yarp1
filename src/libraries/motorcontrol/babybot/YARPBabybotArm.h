#ifndef __YARPBABYBOTARM__
#define __YARPBABYBOTARM__

// $Id: YARPBabybotArm.h,v 1.2 2003-04-29 17:47:18 natta Exp $

#define YARP_BABYBOT_ARM_VERBOSE

#ifdef YARP_BABYBOT_ARM_VERBOSE
#define YARP_BABYBOT_ARM_DEBUG(string) YARP_DEBUG("BABYBOT_ARM_DEBUG:", string)
#else YARP_BABYBOT_ARM_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <YarpGenericComponent.h>
#include "YARPMEIOnBabybotArmAdapter.h"

class YARPBabybotArm : public YARPGenericComponent<YARPMEIOnBabybotArmAdapter, YARPBabybotArmParameters>
{
public:
	int activatePID()
	{
		int ret;
		_lock();
		ret = _adapter.activatePID();
		
		while (!_adapter.checkPowerOn())
		{
			YARP_BABYBOT_ARM_DEBUG(("Press the power on button!\n"));
			ACE_OS::sleep(ACE_Time_Value(1,0));
		}
		_unlock();
		return YARP_OK;
	}
	int calibrate()
	{
		int ret;
		if (! (_initialized && _amplifiers) )
		{
			YARP_BABYBOT_ARM_DEBUG(("Can't calibrate encoders, the arm not initialized or the power is down!\n"));
			return YARP_FAIL;
		}
		bool limitFlag = false;
		if (_sotwareLimits)
			limitFlag = true;
		_adapter.disableLimitCheck();

		for (int i = 0; i < _parameters.nj(); i++)
		{
			
		}


		if (limitFlag)
			_adapter.enableLimitCheck();
		return YARP_OK;
	}
};

#endif