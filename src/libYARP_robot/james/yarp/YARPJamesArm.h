/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #pasa#				              ///
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
///  $Id: YARPJamesArm.h,v 1.2 2006-05-26 23:10:12 babybot Exp $
///
///

#ifndef __YARPJamesArmh__
#define __YARPJamesArmh__


#include <yarp/YARPConfig.h>
#include <yarp/YARPGenericControlBoard.h>
#include <yarp/YARPCanOnJamesArmAdapter.h>

/**
 * \file YARPJamesArm.h It contains definitions and the class defining
 * the abstraction to control the RobotCub hand and arm.
 */

namespace _virtualjoint
{
	const int shoulderRoll = 0;
	const int shoulderPitch = 1;
	const int shouderYaw = 2;
	const int elbow = 3;
	const int wristRoll = 4;
	const int wristPitch = 5;
	const int wristYaw = 6;
	const int thumbRotation = 7;
	const int thumbOpposition = 8;
	const int thumbClose = 9;
	const int indexProximal = 10;
	const int indexDistal = 11;
	const int fingersProximal = 12;
	const int fingersDistal = 13;
	const int handAbduction = 14;
};

namespace _joint
{
	const int shoulderRoll = 0;
	const int shoulderPitch = 1;
	const int shouderYaw = 2;
	const int elbow = 3;
	const int wristRoll = 4;
	const int wristPitch = 5;
	const int wristYaw = 6;
	const int thumbRotation = 7;
	const int thumbOpposition = 8;
	const int thumbClose = 9;
	const int indexProximal = 10;
	const int indexDistal = 11;
	const int fingersProximal = 12;
	const int fingersDistal = 13;
	const int handAbduction = 14;
};


/**
 * YARPJamesArm is the specialization of the generic control board to control
 * the RobotCub arm (as of 2004). It should include things such as kinematic 
 * control of the shoulder, calibration of the joints (where feasible), etc. Also, 
 * it defines a virtual joint control mode where joints are coupled according to the
 * arm/shoulder/hand kinematics to simplify control.
 *
 */
class YARPJamesArm: public YARPGenericControlBoard<YARPCanOnJamesArmAdapter, YARPJamesArmParameters>
{
public:
	// empty for now!
};

#endif

