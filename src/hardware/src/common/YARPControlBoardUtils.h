//
//
// common facilities for control boards
//
// feb 2003 -- by nat and pasa

// $Id: YARPControlBoardUtils.h,v 1.6 2003-05-04 18:09:33 natta Exp $
#include <conf/YARPConfig.h>

#ifndef __YARP_CONTROL_BOARD_UTILS__
#define __YARP_CONTROL_BOARD_UTILS__

#define pi 3.14

// command list
enum ControlBoardCmd
{
	CMDSetSpeed	= 0,		// ref speed for position control
	CMDSetPosition	= 1,	// position control
	CMDSetAcceleration = 2,	// ref acc for positiona and vel control
	CMDSetPID = 3,			// set pid
	CMDGetPosition = 4,		// read actual position
	CMDSetSpeeds = 5,		// see above, all axes
	CMDSetPositions	= 6,	// see above, all axes
	CMDSetAccelerations = 7,// see above, all axes
	CMDGetPositions = 8,	// see above, all axes

	CMDSetOutputPort = 9,	// set output port to a spec value
	CMDSetOutputBit = 10,	// set single bit to 1
	CMDClearOutputBit = 11,	// set single bit to 0

	CMDSetOffset = 12,		// set output offset
	CMDSetOffsets = 13,		// same as above, all axes

	CMDBeginMotion = 14,	// start motion (Galil)
	CMDBeginMotions = 15,	// same all axes

	CMDDefinePosition = 16,	// set encoder, single axis
	CMDDefinePositions = 17,// set encoders
	
	CMDStopAxes= 18,		// issue a stop
	CMDReadSwitches = 19,	// read switched (Galil)
	CMDServoHere = 20,		// set current position as current 
	CMDGetSpeeds= 21,		// istantaneous speed
	
	CMDGetRefSpeeds = 22,			// get reference speed
	CMDGetRefAccelerations = 23,	// get ref acc
	CMDGetRefPositions = 24,		// get ref pos
	
	CMDGetPID = 25,					// read current pid val
	CMDGetTorques = 26,				// read ist torques (output voltage)
	
	CMDSetIntegratorLimits = 27,	// int. limit
	CMDSetTorqueLimits = 28,		// torque limit
	
	CMDGetErrors = 29,				// read current error
	
	CMDReadInput = 30,				// read all ? (not yet impl)
	CMDInitPortAsInput = 31,		// I/O
	CMDInitPortAsOutput = 32,		// I/O
	CMDGetOutputPort = 33,			// I/O
	
	CMDSetAmpEnableLevel = 34,		// set amp enable level
	CMDSetAmpEnable = 35,			// set amp enable
	CMDDisableAmp = 36,				// disable amp
	CMDEnableAmp = 37,				// enable amp
	CMDControllerIdle = 38,			// see MEI
	CMDControllerRun = 39,			// see MEI
	CMDClearStop = 40,				// on MEI -> clear_status
	// limits
	CMDSetPositiveLimit = 41,
	CMDSetNegativeLimit = 42,
	CMDSetPositiveLevel = 43,
	CMDSetNegativeLevel = 44,
	CMDVMove = 45,					// set move vel mode (begin motion)
	CMDCheckMotionDone = 46,		// check for motion done
	CMDWaitForMotionDone = 47,		// wait (loop with sleep)
	CMDSetCommands = 48,			// set current command (watch out! dangerous)
	// home/index 
	CMDSetHomeIndexConfig = 49,
	CMDSetHomeLevel = 50,
	CMDSetHome = 51,
	CMDSetStopRate = 52,			// stop rate1
	CMDDummy = 53,	 // dummy command for debug purpose
	CBNCmds = 54 // required! tells the total number of commands
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

// PID class
struct LowLevelPID
{
	LowLevelPID(){
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