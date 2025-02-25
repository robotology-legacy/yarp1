#ifndef __HEAD_SHARED_DATA
#define __HEAD_SHARED_DATA


typedef YARPHead MY_HEAD;


#include <yarp/YARPBehavior.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPString.h>


class HeadSharedData: public MY_HEAD, public YARPBehaviorSharedData
{
public:
	HeadSharedData():YARPBehaviorSharedData("/headcontrol/behavior/o", "Motor") {}

	void initialize(const YARPString &path, const YARPString &iniFile)
	{
		MY_HEAD::initialize(path, iniFile);

		_status.resize(path, iniFile);
		_directCmd.Resize(_status._nj);
		_inCmd.Resize(_status._nj);
		_predictedPos.Resize(_status._nj);
	}

public:
	YARPControlBoardNetworkData _status;
	YVector _directCmd;
	YVector _inCmd;
	YVector _predictedPos;
};

#endif
