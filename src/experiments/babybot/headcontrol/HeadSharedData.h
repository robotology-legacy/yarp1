#ifndef __HEAD_SHARED_DATA
#define __HEAD_SHARED_DATA

#if defined(__QNXEurobot__)
	#include <YARPEurobotHead.h>
#else
	#include <YARPBabybotHead.h>
#endif

#include <YARPBehavior.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPString.h>

#if defined(__QNXEurobot__)
class HeadSharedData: public YARPEurobotHead, public YARPBehaviorSharedData
#else
class HeadSharedData: public YARPBabybotHead, public YARPBehaviorSharedData
#endif
{
public:
	HeadSharedData():YARPBehaviorSharedData("/headcontrol/behavior/o", "Motor"){};

	void initialize(const YARPString &path, const YARPString &iniFile)
	{
#if defined(__QNXEurobot__)
		YARPEurobotHead::initialize(path, iniFile);
#else
		YARPBabybotHead::initialize(path, iniFile);
#endif
		_status.resize(path, iniFile);
		_directCmd.Resize(_status._nj);
		_inCmd.Resize(_status._nj);
	}

public:
	YARPControlBoardNetworkData _status;
	YVector _directCmd;
	YVector _inCmd;
};

#endif
