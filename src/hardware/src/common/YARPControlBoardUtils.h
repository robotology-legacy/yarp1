//
//
// common facilities for control boards
//
// feb 2003 -- by nat and pasa

#ifndef __YARP_CONTROL_BOARD_UTILS__
#define __YARP_CONTROL_BOARD_UTILS__

#define pi 3.14

// command list
enum ControlBoardCmd
{
	CMDSetSpeed	= 0,
	CMDSetPosition	= 1,
	CMDSetAcceleration = 2,
	CMDSetPID = 3,
	CMDGetPosition = 4,
	
	CMDSetSpeeds = 5,
	CMDSetPositions	= 6,
	CMDSetAccelerations = 7,
	CMDGetPositions = 8,

	CMDSetOutputPort = 9,
	CMDSetOutputBit = 10,
	CMDClearOutputBit = 11,

	CMDSetOffset = 12,
	CMDSetOffsets = 13,

	CMDBeginMotion = 14,
	CMDBeginMotions = 15,

	CMDDefinePosition = 16,
	CMDDefinePositions = 17,
	
	CMDStopAxes= 18,
	CMDReadSwitches = 19,
	CMDServoHere = 20,

	CMDGetSpeeds= 21,
	
	CMDGetRefSpeeds = 22,
	CMDGetRefAccelerations = 23,
	CMDGetRefPositions = 24,
	CMDGetPID = 25,
	CMDGetTorques = 26,

	CMDSetIntegratorLimits = 27,
	CMDSetTorqueLimits = 28,
	CMDGetErrors = 29,

	CMDReadInput = 30,

	CMDInitPortAsInput = 31,
	CMDInitPortAsOutput = 32,
	CMDGetOutputPort = 33,
	CMDSetAmpEnableLevel = 34,
	CMDSetAmpEnable = 35,
	CMDDisableAmp = 36,
	CMDEnableAmp = 37,
	CMDControllerIdle = 38,
	CMDControllerRun = 39,
	CMDClearStop = 40,			// on MEI -> clear_status
	CMDSetPositiveLimit = 41,
	CMDSetNegativeLimit = 42,
	CMDSetPositiveLevel = 43,
	CMDSetNegativeLevel = 44,
	CMDVMove = 45,
	CMDCheckMotionDone = 46,
	CMDWaitForMotionDone = 47,
	CMDSetCommands = 48,
	CMDDummy = 49,	 // dummy command for debug purpose
	CBNCmds = 50 // required! tells the total number of commands
};

enum ControlBoardEvents
{	
	CBNoEvent = 0,
	CBStopEvent = 1,
	CBEStopEvent = 2,
	CBAbortEvent = 3,
	CBNEvents = 4	// required ! tells the total number of events
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

inline double angleToEncoder(double angle, double encParam, int zero, int sign)
{
	if (sign == 1)
		return -(angle * encParam) / (2.0 * pi) + zero;
	else
		return angle * encParam / (2.0 * pi) + zero;
}

inline double encoderToAngle(double encoder, double encParam, int zero, int sign)
{
	if (sign == 1)
		return (-encoder - zero) * 2.0 * pi / encParam;
	else
		return (encoder - zero) * 2.0 * pi / encParam;
}

#endif	//.h