#ifndef __YARPEUROBOTHEAD__
#define __YARPEUROBOTHEAD__

// $Id: YARPEurobotHead.h,v 1.5 2003-12-22 17:57:49 beltran Exp $

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPGALILOnEurobotHeadAdapter.h"

namespace _limits
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int eyeTilt = 2;
	const int eyeVersion = 3;
	const int eyeVergence = 4;
};

namespace _joints
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int eyeTilt = 2;
	const int rightEye = 3;
	const int leftEye = 4;
};

//typedef YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters> YARPEurobotHead;

//typedef YARPGALILOnEurobotHeadAdapter YARPEurobotHead;
class YARPEurobotHead: public YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters>
{
public:
	inline double vergence(double *pos)
	{
		return -( pos[_joints::rightEye] + pos[_joints::leftEye] );
	}
	inline double dot_vergence(double *cmd)
	{
		return cmd[_joints::rightEye] + cmd[_joints::leftEye];
	}

	inline double version(double *pos)
	{
		return 0.5*(pos[_joints::rightEye]-pos[_joints::leftEye]);
	}
	inline double dot_version(double *cmd)
	{
		return 0.5*(cmd[_joints::rightEye]-cmd[_joints::leftEye]);

	}
	inline bool checkLimits(double *pos, double *cmd)
	{
		bool ret = false;
		bool tmp = false;
	
		// check eyes, vergence and version
		tmp = checkSingleJoint(vergence(pos), dot_vergence(cmd), _parameters._limitsMax[_limits::eyeVergence], _parameters._limitsMin[_limits::eyeVergence], -1);
		tmp |= checkSingleJoint(version(pos), dot_version(cmd), _parameters._limitsMax[_limits::eyeVersion], _parameters._limitsMin[_limits::eyeVersion], 1);
		if (tmp)
		{
			cmd[_joints::rightEye] = 0.0;
			cmd[_joints::leftEye] = 0.0;
			ret = true;
		}

		// check eyes, tilt
		tmp = checkSingleJoint(pos[_joints::eyeTilt], cmd[_joints::eyeTilt], _parameters._limitsMax[_limits::neckPan], _parameters._limitsMin[_limits::eyeTilt], 1);
		if (tmp)
		{
			cmd[_joints::eyeTilt] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joints::neckPan], cmd[_joints::neckPan], _parameters._limitsMax[_limits::neckPan], _parameters._limitsMin[_limits::neckPan], 1);
		if (tmp)
		{
			cmd[_joints::neckPan] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joints::neckTilt], cmd[_joints::neckTilt], _parameters._limitsMax[_limits::neckTilt], _parameters._limitsMin[_limits::neckTilt], 1);
		if (tmp)
		{
			cmd[_joints::neckTilt] = 0.0;
			ret = true;
		}
	
		return ret;
	}

	// check limits on a sigle joints
	inline bool checkSingleJoint(double pos, double cmd, double up, double low, int sign)
	{
		if (sign > 0)
		{
			if ((pos < low && cmd < 0) || (pos > up && cmd > 0))
				return true;
			else
				return false;
		}
		else
		{
			if ((pos < low && cmd > 0) || (pos > up && cmd < 0))
				return true;
			else
				return false;
		}
	}

};

#endif
