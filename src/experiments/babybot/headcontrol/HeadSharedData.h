#ifndef __HEAD_SHARED_DATA
#define __HEAD_SHARED_DATA

#include <YARPBabybothand.h>
#include <YARPBehavior.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPString.h>

class HeadSharedData: public YARPBabybotHead, public YARPBehaviorSharedData
{
public:
	HeadSharedData():YARPBehaviorSharedData(YBLabelMotor, "/headcontrol/behavior/o"){};

	void initialize(const YARPString &path, const YARPString &iniFile)
	{
		YARPBabybotHead::initialize(path, iniFile);
		_status.resize(path, iniFile);
	}

public:
	YARPControlBoardNetworkData _status;
};

#endif
