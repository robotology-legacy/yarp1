/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///                                                                   ///
/// This Academic Free License applies to any software and associated ///
/// documentation (the "Software") whose owner (the "Licensor") has   ///
/// placed the statement "Licensed under the Academic Free License    ///
/// Version 1.0" immediately after the copyright notice that applies  ///
/// to the Software.                                                  ///
/// Permission is hereby granted, free of charge, to any person       ///
/// obtaining a copy of the Software (1) to use, copy, modify, merge, ///
/// publish, perform, distribute, sublicense, and/or sell copies of   ///
/// the Software, and to permit persons to whom the Software is       ///
/// furnished to do so, and (2) under patent claims owned or          ///
/// controlled by the Licensor that are embodied in the Software as   ///
/// furnished by the Licensor, to make, use, sell and offer for sale  ///
/// the Software and derivative works thereof, subject to the         ///
/// following conditions:                                             ///
/// Redistributions of the Software in source code form must retain   ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers. ///
/// Redistributions of the Software in executable form must reproduce ///
/// all copyright notices in the Software as furnished by the         ///
/// Licensor, this list of conditions, and the following disclaimers  ///
/// in the documentation and/or other materials provided with the     ///
/// distribution.                                                     ///
///                                                                   ///
/// Neither the names of Licensor, nor the names of any contributors  ///
/// to the Software, nor any of their trademarks or service marks,    ///
/// may be used to endorse or promote products derived from this      ///
/// Software without express prior written permission of the Licensor.///
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
/// This license is Copyright (C) 2002 Lawrence E. Rosen. All rights  ///
/// reserved. Permission is hereby granted to copy and distribute     ///
/// this license without modification. This license may not be        ///
/// modified without the express written permission of its copyright  ///
/// owner.                                                            ///
///                                                                   ///
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
/// $Id: YARPEurobotHead.h,v 1.10 2004-01-17 00:15:15 gmetta Exp $
///
///

#ifndef __YARPEUROBOTHEAD__
#define __YARPEUROBOTHEAD__

#include <conf/YARPConfig.h>
#include <YARPGenericControlBoard.h>
#include "YARPGALILOnEurobotHeadAdapter.h"

namespace _limits
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int eyeVersion = 2;
	const int eyeVergence = 3;
};

namespace _joints
{
	const int neckPan = 0;
	const int neckTilt = 1;
	const int rightEye = 2;
	const int leftEye = 3;
};

//typedef YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters> YARPEurobotHead;

//typedef YARPGALILOnEurobotHeadAdapter YARPEurobotHead;
class YARPEurobotHead: public YARPGenericControlBoard<YARPGALILOnEurobotHeadAdapter, YARPEurobotHeadParameters>
{
public:
	inline double vergence(double *pos)
	{
		return -( pos[_joints::rightEye] - pos[_joints::leftEye] );
	}
	inline double dot_vergence(double *cmd)
	{
		return cmd[_joints::rightEye] - cmd[_joints::leftEye];
	}

	inline double version(double *pos)
	{
		return 0.5*(-pos[_joints::rightEye]+pos[_joints::leftEye]);
	}
	inline double dot_version(double *cmd)
	{
		return 0.5*(-cmd[_joints::rightEye]+cmd[_joints::leftEye]);

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
