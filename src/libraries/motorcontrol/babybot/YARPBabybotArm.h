#ifndef __YARPBABYBOTARM__
#define __YARPBABYBOTARM__

// $Id: YARPBabybotArm.h,v 1.4 2003-04-30 16:03:19 natta Exp $

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
	// override activatePID
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
		_lock();
		ret = _adapter.calibrate();
		_unlock();
		return ret;
	}
};

#endif

