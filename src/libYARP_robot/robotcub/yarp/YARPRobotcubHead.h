/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #pasa#				                          ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #Add our name(s) here#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
///  $Id: YARPRobotcubHead.h,v 1.1 2004-07-28 17:17:35 babybot Exp $
///
///

#ifndef __YARPRobotcubHeadh__
#define __YARPRobotcubHeadh__


#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include <YARPCanOnRobotcubHeadAdapter.h>

///
///
///
namespace _virtualjoint
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int eyeTilt = 2;
	const int eyeVersion = 3;
	const int eyeVergence = 4;
};

namespace _joint
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int eyeTilt = 2;
	const int rightEye = 3;
	const int leftEye = 4;
};

///
///
///
class YARPRobotcubHead: public YARPGenericControlBoard<YARPCanOnRobotcubHeadAdapter, YARPRobotcubHeadParameters>
{
public:
 	inline double vergence(double *pos)
	{
		return -( pos[_joint::rightEye] + pos[_joint::leftEye] );
	}

	inline double dot_vergence(double *cmd)
	{
		return cmd[_joint::rightEye] + cmd[_joint::leftEye];
	}

	inline double version(double *pos)
	{
		return 0.5*(pos[_joint::rightEye]-pos[_joint::leftEye]);
	}

	inline double dot_version(double *cmd)
	{
		return 0.5*(cmd[_joint::rightEye]-cmd[_joint::leftEye]);

	}

	inline bool checkLimits(double *pos, double *cmd)
	{
		bool ret = false;
		bool tmp = false;
	
		// check eyes, vergence and version
		tmp = checkSingleJoint(vergence(pos), dot_vergence(cmd), _parameters._limitsMax[_virtualjoint::eyeVergence], _parameters._limitsMin[_virtualjoint::eyeVergence], -1);
		tmp |= checkSingleJoint(version(pos), dot_version(cmd), _parameters._limitsMax[_virtualjoint::eyeVersion], _parameters._limitsMin[_virtualjoint::eyeVersion], 1);
		if (tmp)
		{
			cmd[_joint::rightEye] = 0.0;
			cmd[_joint::leftEye] = 0.0;
			ret = true;
		}

		// check eyes, tilt
		tmp = checkSingleJoint(pos[_joint::eyeTilt], cmd[_joint::eyeTilt], _parameters._limitsMax[_virtualjoint::neckPan], _parameters._limitsMin[_virtualjoint::eyeTilt], 1);
		if (tmp)
		{
			cmd[_joint::eyeTilt] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joint::neckPan], cmd[_joint::neckPan], _parameters._limitsMax[_virtualjoint::neckPan], _parameters._limitsMin[_virtualjoint::neckPan], 1);
		if (tmp)
		{
			cmd[_joint::neckPan] = 0.0;
			ret = true;
		}

		tmp = checkSingleJoint(pos[_joint::neckTilt], cmd[_joint::neckTilt], _parameters._limitsMax[_virtualjoint::neckTilt], _parameters._limitsMin[_virtualjoint::neckTilt], 1);
		if (tmp)
		{
			cmd[_joint::neckTilt] = 0.0;
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