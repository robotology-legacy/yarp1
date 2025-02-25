/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
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
/// $Id: YARPControlBoardUtils.h,v 1.20 2006-06-13 12:31:46 gmetta Exp $
///
///

//
//
// common facilities for control boards
//
// feb 2003 -- by nat and pasa


#include <yarp/YARPConfig.h>


#ifndef __YARP_CONTROL_BOARD_UTILS__
#define __YARP_CONTROL_BOARD_UTILS__


/**
 * ControlBoardCmd is the list of possible commands for a generic 
 * motor control card.
 *
 */
enum ControlBoardCmd
{
	//
	// set/get state information about a specific axis or about many axes.
	//
	CMDDefinePosition 		= 16,	// set encoder value, single axis.
	CMDDefinePositions 		= 17,	// set encoder values, all axes simultaneously.
	CMDGetPosition 			= 4,	// read actual encoder position.
	CMDGetPositions 		= 8,	// see above, all axes.
	CMDSetSpeed				= 0,	// set the reference speed for position control.
	CMDSetSpeeds 			= 5,	// see above, all axes.
	CMDGetSpeed				= 20,	// istantaneous speed, read from encoder (single joint)
	CMDGetSpeeds			= 21,	// istantaneous speed, read from encoders.
	// CMDGetAcceleration	=	,	// istantaneous acceleration of a single axis.
	CMDGetAccelerations		= 90,   // istantaneous acceleration for all axes.
	CMDSetAcceleration 		= 2,	// reference acceleration for position and velocity control.
	CMDSetAccelerations 	= 7,	// see above, all axes.
	CMDGetRefPosition		= 84,	// get the reference (commanded) position for a single axis.
	CMDGetRefPositions 		= 24,	// get reference position (commanded).
	CMDGetPIDError			= 85,	// get the PID error.
	CMDGetPIDErrors			= 102,	// get the PID error, vector version
	CMDGetRefSpeeds 		= 22,	// get reference speed.
	CMDGetRefAccelerations 	= 23,	// get reference acceleration.
	CMDSetCommands 			= 50,	// set current commands (watch out! dangerous)
	CMDSetCommand 			= 51,	// set current commands (watch out! dangerous)
	CMDGetSWPositiveLimit 	= 79,	// gets positive limit value.
	CMDGetSWNegativeLimit 	= 80,	// gets negative limit value.
	CMDSetSWPositiveLimit 	= 81,	// sets positive limit value.
	CMDSetSWNegativeLimit 	= 82,	// sets negative limit value.

	//
	// set/get the PID gain values including limits.
	//
	CMDSetPID 				= 3,	// set the PID for a given axis.
	CMDGetPID 				= 25,	// read current PID gain values.
	CMDSetOffset 			= 12,	// set the output offset.
	CMDSetOffsets 			= 13,	// same as above, all axes.
	CMDSetIntegratorLimit   = 29,	// integrator limit, single joint.
	CMDSetIntegratorLimits 	= 27,	// integrator limit, all axes.
	CMDGetTorque			= 83,	// read the output from a single axis.
	CMDGetTorques 			= 26,	// read the output voltage for all axes.
	CMDSetTorqueLimit		= 30,   // torque limit, single joint (PWM maximum value).
	CMDSetTorqueLimits 		= 28,	// torque limit, all axes.
	CMDGetTorqueLimit		= 60,   // get torque limit, single joint
	CMDGetTorqueLimits		= 61,   // get torque limit, multiple joints
	CMDGetPWM				= 88,	// get pwm output
	CMDGetPWMs				= 89,	// get pwm output multiple joints
	CMDSetCurrentLimit		= 93,	// set the maximum current threshold (the exact numeric value depends on the card)
									// this is typically measured internally by the control card.
	CMDSetCurrentLimits		= 94,	// sets the maximum current threshold for all joints.
	CMDSetErrorLimit		= 58,	// sets the position error threshold.

	//
	// actual motion commands.
	//
	CMDSetPosition			= 1,	// position control (start a movement).
	CMDSetPositions			= 6,	// sets reference position for movement, all axes.
	CMDSetPositionRelative	= 91,   // peform relative motion on a single axis.
	CMDSetPositionsRelative = 92,   // perform relative motion on multiple axis (LATER: name to be standardized!).
	CMDCheckMotionDone 		= 48,	// check for motion done - if the previous command required trajectory generation.
    CMDCheckMotionDoneSingle = 15,  // checl for motion done on a single axis.
	CMDWaitForMotionDone 	= 49,	// wait (loop with sleep) - polling at a certain rate.
	// CMDStopAxis			=   ,	// issues a stop command to a single axis.
	CMDStopAxes				= 18,	// issues a stop command to all axes.
	CMDClearStop 			= 42,	// clear the error status (depending on the card, a sw stop generates a fault situation).
	CMDVMove 				= 47,	// set move using the velocity mode (begin motion).

	//
	// Analog I/O commands.
	//
	CMDGetAnalogChannel		= 66,	// read analog input from one axis.
	CMDGetAnalogChannels	= 31,	// read analog input from all axes.
	CMDSetAxisAnalog		= 67,	// configure axis analog.

	//
	// Digital I/O commands.
	//
	CMDSetPortValue			= 9,	// set output port to a specific value.
	CMDGetPortValue			= 35,	// read value from an input port.
	CMDSetOutputBit 		= 10,	// set single output bit to 1.
	CMDClearOutputBit 		= 11,	// set single output bit to 0.
	//CMDReadInput 			= 32,	// ???
	CMDInitPortAsInput 		= 33,	// sets port as input.
	CMDInitPortAsOutput 	= 34,	// sets port as output.

	//
	// Amplifier/PWM control.
	//
	CMDSetAmpEnableLevel 	= 36,	// set amplifier enable level (high/low).
	CMDSetAmpEnable 		= 37,	// set amplifier enable.
	CMDDisableAmp 			= 38,	// disable amplifier.
	CMDEnableAmp 			= 39,	// enable amplifier.
	CMDControllerIdle 		= 40,	// disables the PID computation and output.
	CMDControllerRun 		= 41,	// starts the PID computation and output.

	//
	// Internal event handling.
	//
	CMDSetPositiveLimit 	= 43,	// sets positive limit event (what to do in case a limit is reached).
	CMDSetNegativeLimit 	= 44,	// sets negative limit event.
	CMDSetPositiveLevel 	= 45,	// sets level signal for limit switch.
	CMDSetNegativeLevel 	= 46,	// sets level signal for limit switch.
	CMDOffOnError			= 59,	// this command causes the controller to shut off
									// the  motor command if a position error exceeds
									// the limit specified by the CMDSetErrorLimit command

	//
	// Miscellaneous.
	//
	CMDSetPositionControlMode = 86,   // set position mode.
	CMDSetForceControlMode	  = 87,	  // set force control mode.
	// CMDSetVelocityControlMode = ,
    CMDStartCalibration     = 104,  // start calibration (homing procedure) of a single joint.
    CMDSetHomingBehavior	= 52,	// sets the homing behavior during calibration (search what, index, etc.)
									// it's doubtful whether this command is to be maintained.
	CMDResetController 		= 57,	// reset the controller.
	CMDGetErrorStatus		= 74,	// gets the error status of the control card.
	CMDGetFault				= 101,  // gets fault events for a single axis.
	CMDGetFaults			= 103,  // gets fault events for all axes.
    CMDGetControlMode       = 14,   // gets the control mode (velocity, position, etc). 
	CMDLoadBootMemory		= 72,	// loads control values from permanent storage.
	CMDSaveBootMemory		= 73,	// saves important parameters to permanent storage.

	//
	// Specific to the device driver behavior (nothing is sent to the card) - used for debugging.
	//
	CMDSetDebugMessageFilter	= 77,	// sets the debug message filter.
	CMDSetDebugPrintFunction	= 78,	// sets the debug print function.
	
	//
	// Make sure the CBNCmds is always up to date.
	//
	CBNCmds 				= 105 	// required! tells the total number of commands
};


enum ControlBoardEvents
{	
	CBNoEvent = 0,
	CBStopEvent = 1,
	CBEStopEvent = 2,
	CBAbortEvent = 3,
	CBNEvents = 4	// required ! tells the total number of events
};

enum ControlBoardHomeConfig
{
	CBHomeOnly = 0,
	CBLowHomeAndIndex = 1,
	CBIndexOnly = 2,
	CBHighHomeAndIndex = 3,
	CBNHomeConfig = 4	// required for future use ?
};


/**
 * LowLevelPID is a simple PID class to contain the control board
 * PID parameters. This class is not doing any computation, it is just
 * a storage for managing the parameters.
 */
struct LowLevelPID
{
	LowLevelPID()
	{
		KP = 0.0;
		KD = 0.0;
		KI = 0.0;
		AC_FF = 0.0;
		VEL_FF = 0.0;
		I_LIMIT = 0.0;
		OFFSET = 0.0;
		T_LIMIT = 0.0;
		SHIFT = 0.0;
		FRICT_FF = 0.0;
	}

	LowLevelPID(double kp , double kd , double ki , double ac_ff, double vel_ff,
				double i_limit, double offset, double t_limit, double shift, double frict_ff)
	{
		KP = kp;
		KD = kd;
		KI = ki;
		AC_FF = ac_ff;
		VEL_FF = vel_ff;
		I_LIMIT = i_limit;
		OFFSET = offset;
		T_LIMIT = t_limit;
		SHIFT = shift;
		FRICT_FF = frict_ff;
	}

	LowLevelPID(double *pid_par)
	{
		KP = pid_par[0];
		KD = pid_par[1];
		KI = pid_par[2];
		AC_FF = pid_par[3];
		VEL_FF = pid_par[4];
		I_LIMIT = pid_par[5];
		OFFSET = pid_par[6];
		T_LIMIT = pid_par[7];
		SHIFT = pid_par[8];
		FRICT_FF = pid_par[9];
	}

	double KP;
	double KD;
	double KI;
	double AC_FF;
	double VEL_FF;
	double I_LIMIT;
	double OFFSET;
	double T_LIMIT;
	double SHIFT;
	double FRICT_FF;
};

// operator overload for the LowLevelPID class. These are used only within 
// the "setGainsSmoothly" functions (see code). SHIFT and OFFSET are not
// considered (they should be both 0 as set by the constructor).
LowLevelPID operator -(const LowLevelPID &l, const LowLevelPID &r);
LowLevelPID operator +(const LowLevelPID &l, const LowLevelPID &r);
LowLevelPID operator /(const LowLevelPID &l, const double v);
LowLevelPID operator *(const LowLevelPID &l, const double v);

// single axis params
struct SingleAxisParameters
{
	int axis;
	void *parameters;
};

struct IOParameters
{
	int port;
	short value;
};

#endif	//.h
