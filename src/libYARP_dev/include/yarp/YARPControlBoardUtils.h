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
/// $Id: YARPControlBoardUtils.h,v 1.14 2006-02-10 08:28:48 gmetta Exp $
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
	// set/get state information about a specific axis or about
	// many axes including position limit values.
	//
	CMDDefinePosition 		= 16,	// set encoder value, single axis.
	CMDDefinePositions 		= 17,	// set encoder values, all axes simultaneously.

	CMDGetPosition 			= 4,	// read actual encoder position.
	CMDGetPositions 		= 8,	// see above, all axes.
	
	CMDSetSpeed				= 0,	// set the reference speed for position control.
	CMDSetSpeeds 			= 5,	// see above, all axes.
	CMDGetSpeed				= 87,	// istantaneous speed, read from encoder (single joint)
	CMDGetSpeeds			= 21,	// istantaneous speed, read from encoders.

	CMDGetAccelerations		= 90,   // istantaneous acc, from encoders
	CMDSetAcceleration 		= 2,	// reference acceleration for position and velocity control.
	CMDSetAccelerations 	= 7,	// see above, all axes.

	CMDGetRefPosition		= 84,	// get the reference (commanded) position for a single axis.
	CMDGetRefPositions 		= 24,	// get reference position (commanded).
	CMDGetPIDError			= 85,	// get the PID error.

	CMDGetRefSpeeds 		= 22,	// get reference speed.
	CMDGetRefAccelerations 	= 23,	// get reference acceleration.

	CMDSetCommands 			= 50,	// set current commands (watch out! dangerous)
	CMDSetCommand 			= 51,	// set current commands (watch out! dangerous)

	CMDGetSWPositiveLimit 	= 79,	// gets positive limit value.
	CMDGetSWNegativeLimit 	= 80,	// gets negative limit value.
	CMDSetSWPositiveLimit 	= 81,	// sets positive limit value.
	CMDSetSWNegativeLimit 	= 82,	// sets negative limit value.

	CMDSetPositiveLimit 	= 43,	// sets positive limit event (what to do in case a limit is reached).
	CMDSetNegativeLimit 	= 44,	// sets negative limit event.
	CMDSetPositiveLevel 	= 45,	// sets level signal for limit switch.
	CMDSetNegativeLevel 	= 46,	// sets level signal for limit switch.

	//
	//
	// set/get the PID gain values including limits.
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
	CMDSetCurrentLimits		= 94,	// set the maximum current threshold for all joints.

	//
	// actual motion commands.
	//
	CMDSetPosition			= 1,	// position control (start a movement).
	CMDSetPositions			= 6,	// see above, all axes.
	CMDBeginMotion 			= 14,	// start motion (certain cards require explicit start command).
	CMDBeginMotions 		= 15,	// same as above but all axes.

	CMDStopAxes				= 18,	// issue a stop command to all axes.
	CMDSetStopRate 			= 55,	// stop rate, at what rate to stop motion (deceleration).
	CMDServoHere 			= 20,	// set current position as current.
	CMDClearStop 			= 42,	// clear the error status (depends on the card).

	CMDVMove 				= 47,	// set move using the velocity mode (begin motion).
	CMDSafeVMove			= 71,	// velocity move, check frames left before submitting command.

	CMDRelativeMotion		= 91,   // peform relative motion on a single axis.
	CMDRelativeMotionMultiple       = 92,   // perform relative motion on multiple axis (LATER: name to be standardized!).

	CMDCheckMotionDone 		= 48,	// check for motion done.
	CMDWaitForMotionDone 	= 49,	// wait (loop with sleep).

	//
	// I/O commands.
	//
	CMDSetOutputPort 		= 9,	// set output port to a specific value.
	CMDSetOutputBit 		= 10,	// set single bit to 1.
	CMDClearOutputBit 		= 11,	// set single bit to 0.
	
	CMDReadSwitches 		= 19,	// read switches (limit or other).
	
	CMDReadAnalog			= 66,	// read analog input.
	CMDSetAxisAnalog		= 67,	// configure axis analog.

	CMDReadInput 			= 32,	// read all ? (not yet impl)
	CMDInitPortAsInput 		= 33,	// digital I/O 
	CMDInitPortAsOutput 	= 34,	// digital I/O
	CMDGetOutputPort 		= 35,	// digital I/O

	CMDGetErrors 			= 31,	// read current error.
	CMDAbortAxes			= 62,	// abort motion.

	CMDSetHomeIndexConfig 	= 52,
	CMDSetHomeLevel 		= 53,
	CMDSetHome 				= 54,
	CMDIndexSearch		    = 56,	// Index Search (Index search + jog move = search for indexes)

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
	// Miscellaneous.
	//
	CMDSetPositionControlMode = 86,   // set position mode.
	CMDSetForceControlMode	  = 87,	  // set force control mode.

	CMDResetController 		= 57,	// reset the controller.
	CMDErrorLimit			= 58,	// PID error limit (position error threshold).
	CMDOffOnError			= 59,	// this command causes the controller to shut off
									// the  motor command if a position error exceeds
									// the limit specified by the CMDErrorLimit command

	CMDMotorType			= 64,	// used to set the motor type (e.g. DC, stepper) - depends on the card.
	CMDGetMotorType			= 65,

	CMDSetDR				= 68,	// it configures the second communication channel and the data update (WARNING: specific to Galil).
	CMDCheckFramesLeft		= 69,	// return true if frames are left to be executed for one or more axes (for buffered communication).
	CMDWaitForFramesLeft	= 70,	// wait, loop with sleep(time), see .h for details

	CMDLoadBootMemory		= 72,	// loads control values from permanent storage.
	CMDSaveBootMemory		= 73,	// saves important parameters to permanent storage.
	CMDGetErrorStatus		= 74,	// gets the error status of the control card.
	CMDSetBoardID			= 75,	// broadcasts a set board ID command.
	CMDGetBoardID			= 76,	// broadcasts a get board ID command.


	//

	// Broadcast management of messages (on interrupt/event based).

	//
	CMDSetBCastMsgs			= 95,	// prepares the remote to broadcast certain info back automatically.
	CMDGetBCastPositions	= 96,	// reads the values of the positions.
	CMDGetBCastVelocities	= 97,	// ditto for velocities.
	CMDGetBCastAccelerations= 98,	// accelerations.
	CMDGetBCastCurrents		= 99,	// reads the values of the current consumption.
	CMDGetBCastFaultsAndReset = 100, // reads the faults and reset them (until next fault is generated - one shot event).

	CMDGetBCastControlValues = 101,	 // reads the control values from the internal (PID?) controller.


	//
	// Specific to the device driver behavior (nothing is sent to the card) - used for debugging.
	//
	CMDSetDebugMessageFilter = 77,	// sets the debug message filter.
	CMDSetDebugPrintFunction = 78,	// sets the debug print function.
	CMDDummy 				 = 63,	// dummy command for debug purposes.
	
	//
	// Make sure the CBNCmds is always up to date.
	//
	CBNCmds 				= 102 	// required! tells the total number of commands
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
