#ifndef __HEAD_SHARED_DATA
#define __HEAD_SHARED_DATA


#if defined(__QNXEurobot__)
	#include <YARPEurobotHead.h>
	typedef YARPEurobotHead MY_HEAD;
#else
	#include <YARPBabybotHead.h>
	typedef YARPBabybotHead MY_HEAD;
#endif


#include <YARPBehavior.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPString.h>


class HeadSharedData: public MY_HEAD, public YARPBehaviorSharedData
{
public:
	HeadSharedData():YARPBehaviorSharedData("/headcontrol/behavior/o", "Motor"){};

	void initialize(const YARPString &path, const YARPString &iniFile)
	{
		MY_HEAD::initialize(path, iniFile);

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
