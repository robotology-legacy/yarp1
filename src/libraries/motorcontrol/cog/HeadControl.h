// 
// HeadControl.h
//
#ifndef __HeadControlh__
#define __HeadControlh__

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <sys/kernel.h>

#include "YARPSafeNew.h"
#include "YARPBool.h"
#include "YARPSemaphore.h"
#include "YARPTime.h"

#include "isd.h"
#include "meid.h"
#include "mei/pcdsp.h"

#include "CogGaze.h"

#define DEFAULTFILENAME "/ai/ai/mb/cdp/src/YARP/conf/head.ini"
//#define DEFAULTDEVICENAME "/yarp/mei/head"
#define DEFAULTDEVICENAME "//4/dev/mei0x320"

//
// LATER: The base class is not that useful. It might be removed in the future.
// it just keeps a little of separation between the MEI stuff and the
//	high level calls.
//

class YARPGenericControl 
{
private:
	YARPGenericControl (const YARPGenericControl&);
	void operator= (const YARPGenericControl&);

protected:
	static int meidx;		// meid handle.

	static int m_instances;
	static YARPSemaphore m_mutex;		// mutex. 

	inline void Lock(void) { m_mutex.Wait ();	}
	inline void Unlock(void) { m_mutex.Post (); }

public:
	YARPGenericControl ();
	virtual ~YARPGenericControl();

	// protected by now. I don't know yet whether we need these callable
	// from user's code.
protected:
	virtual int Initialize (void);
	virtual int Initialize (const char *mei_name);
	virtual int Uninitialize (void);

	virtual int Reset (void) { return DspReset(); }

	/* message-passing wrappers */
	inline int16 DspReset (void);
	inline int16 StartMove (int16 axis, double position, double velocity, double acceleration);
	inline int16 StartMoveAll (int16 len, int16 *axes, double *position, double *velocity, double *acceleration);
	inline int16 GetVelocity (int16 axis, double *velocity);
	inline int16 GetAccel (int16 axis, double *accel);
	inline int16 GetJerk (int16 axis, double *jerk);
	inline int16 GetCommand (int16 axis, double *command);
	inline int16 GetPosition (int16 axis, double *position);
	inline int16 GetError (int16 axis, double *error);
	inline int16 GetFilter (int16 axis, int16 *coeffs);
	inline int16 SetFilter (int16 axis, int16 *coeffs);
	inline int16 GetInPosition (int16 axis, double *limit);
	inline int16 SetInPosition (int16 axis, double limit);
	inline int16 FramesLeft (int16 axis);
	inline int16 FramesToExecute (int16 axis);
	inline int16 AxisStatus (int16 axis);
	inline int16 AxisState (int16 axis);
	inline int16 AxisDone (int16 axis);
	inline int16 AxisSource (int16 axis);
	inline int16 DspVelocity (int16 axis, double velocity, double duration);
	inline int16 MeiLink (int16 master, int16 slave, double ration, int16 source);
	inline int16 EndLink (int16 slave);
	inline int16 SetVelocity (int16 axis, double vel);
	inline int16 ZeroAxis (int16 axis);
	inline int16 GetBootFilter (int16 axis, int16 *coeffs);
	inline int16 SetBootFilter (int16 axis, int16 *coeffs);
	inline int16 Checksum(void);
	inline int16 GetAnalog (int16 axis, int16 *value);
	inline int16 GetAllAnalog (int *values);
	inline int16 SetDacOutput (int16 axis, int16 power);
	inline int16 ControllerIdle (int16 axis);
	inline int16 SetDualLoop (int16 axis, int16 vel_axis, int16 dual);
	inline int16 ReadAxisAnalog (int16 axis, int16 *value);
	inline int16 SetFeedback (int16 axis, int16 device);
	inline int16 SetAxisAnalog (int16 axis, int16 state);
	inline int16 SetAnalogChannel (int16 axis, int16 channel, int16 differential, int16 bipolar);
	inline int16 SetCommand (int16 axis, double position);
	inline int16 SetEStop (int16 axis);
	inline int16 SetStop (int16 axis);
	inline int16 ClearStatus (int16 axis);
	inline int16 MotionDone (int16 axis);

	inline int16 VMove (int16 axis, double velocity, double acceleration);
};

//
// Assuming boot memory is correct.
// It may be checked during initialization.
//
typedef enum { ModeReal, ModeVirtual } EHeadControlMode;

//
// All encoder positions are actual (motor level).
// Motor commands are tranformed in motor level values before
// being sent to MEI.
//

// Be aware that virtual axes support is maintained by the derived class.
// calling base class members does not provide virtual axes control.
// These methods are now protected. If necessary override them to
// add that particular functionality to YARPHeadControl

class YARPHeadControl : public YARPGenericControl
{
private:
	static double *m_old_pos;
	static clock_t m_timestamp;

	double *m_set_cmd;
	double *m_set_vel;
	double *m_set_acc;
	short *m_winding;
	double *m_16bit_oldpos;

	// additional check of limits.
	CogGaze m_gaze;
	JointPos m_pos;
	int VerifyLimits (double interval);

	void _out_vector (int len, double* v);

protected:
	static int16 **m_boot_filter;		// axes * COEFF.
	static int16 **m_control_params;

	static int *m_enabled;				// axes.
	static bool *m_calibrated;			// calibrated axis.

	// limits?
	static double *m_limits_max;		// limits.
	static double *m_limits_min;
	static double *m_pad_max;
	static double *m_pad_min;

	// all these are in virtual coordinates.
	static double *m_default_vel;		// default values (for pos. move)
	static double *m_default_acc;		// for both position and v_move.
	static double *m_default_jerk;		// for s-profile move.

	static double *m_position;			// actual position.
	static double *m_velocity;			// actual velocity.
	static double *m_calibrate_offset;	// axes.
	static double *m_max_error;			// max calibration error.
	static double *m_position_zero;

	// encoders to angles in degrees.
	static double *m_etoangles;	
	
	// power down/up by raising, lowering the MEI gains.
	static bool m_shutdown_flag;
	static double m_tilt_store;

	static EHeadControlMode m_mode;
	
	int SetRobotSpecific (void);
	int CheckBootParams (void);
	double VelocityMove (int axis, double vel, double acc, double errlimit);

	static int m_njoints;

	static int m_eyetilt;
	static int m_righteye;
	static int m_lefteye;
	static int m_topdiff;
	static int m_bottomdiff;
	static int m_neckpan;
	static int m_necktilt;

	static int m_headtilt;
	static int m_headroll;

	void ReadConfig (const char *filename);

	// non public methods. Likely not thread-safe.
	// helper functions.
protected:
	virtual int CheckAxis (int i, double command, double interval);
	virtual int CheckAxisPos (int i, double command);

	void EatLine (FILE *fp);	// helper function.
	inline int16 ZeroAxis (int16 axis)
	{
		int16 ret = YARPGenericControl::ZeroAxis (axis);
		m_position_zero[axis] = double(meid_dsp_encoder(meidx, axis));
		return ret;
	}

public:
	enum { MaxAxes = 8, TILTSTORE = -100000 };

	// axis numbers.
	int EyeTilt() const { return m_eyetilt; }
	int RightEye() const { return m_righteye; }
	int LeftEye() const { return m_lefteye; }
	int TopDifferential() const { return m_topdiff; }
	int BottomDifferential() const { return m_bottomdiff; }
	int NeckPan() const { return m_neckpan; }
	int NeckTilt() const { return m_necktilt; }

	// virtual axis.
	int HeadTilt() const { return m_headtilt; }
	int HeadRoll() const { return m_headroll; }

	int GetNJoints() const { return m_njoints; }

	YARPHeadControl ();
	virtual ~YARPHeadControl ();

	virtual int Initialize (void);
	virtual int Initialize (const char *name, const char *filename = NULL);
	virtual int Uninitialize (void);

	// specific to this head.
	virtual int Calibrate (int axis);
	virtual int Calibrate (int numaxes, int *axes);
	virtual int DifferentialCalibration (void);

	inline void ConvertToReal(double nod, double roll, double *top, double *bot);
	inline void ConvertToVirtual(double top, double bot, double *nod, double *roll);

	// all these are in virtual coordinates.
	virtual int Input (void);
	virtual int PositionCommand (double *command, int *flags = NULL);
	virtual int VelocityCommand (double *command, double interval);
	virtual bool AllDone (void);

	virtual int GetActualPosition (double *position);
	virtual int GetActualPosition (int vaxis, double* position);
	virtual int GetActualVelocity (double *velocity);
	virtual int GetActualVelocity (int vaxis, double* velocity);
	virtual int SetDefaultVelocity (double *velocity);
	virtual int SetDefaultVelocity (int vaxis, double velocity);
	virtual int SetDefaultAcc (double *acc);
	virtual int SetDefaultAcc (int vaxis, double acc);

	virtual int EnableAxis (int vaxis);
	virtual int DisableAxis (int vaxis);
	virtual int SetCalibrationFlag (int vaxis, bool flag);
	virtual bool IsCalibrated (int vaxis);
	virtual int SetLimits (int vaxis, double min, double max);
	virtual int GetLimits (int vaxis, double *min, double *max);

	virtual int SetZeroHere (int vaxis);
	virtual int SetZeroHere (void);
	virtual int ResetAllEncoders (void);

	virtual int ParkHead (void);
	virtual int UnparkHead (void);
	virtual int LowerGains (void);
	virtual int RaiseGains (void);
};

#include "HeadControl.inl"

#endif
