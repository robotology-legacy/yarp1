//
//
// ArmControl.h
//	wrapper class(es) for arm/torso/hand control.
//

#ifndef __ArmControlh__
#define __ArmControlh__

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <sys/kernel.h>

#include "YARPSafeNew.h"
#include "YARPBool.h"
#include "YARPSemaphore.h"
#include "YARPRateThread.h"

// from models.
#include "LowPassFilter.h"
#include "LookupTable.h"

#include "meid.h"
#include "ueid.h"

enum __jointtypes
{
	TROLL=0,		// mei node 3 axis 0
	TPITCH=1,
	TYAW=2,
	LSHOULDERA=3,	// mei node 7 axis
	LSHOULDERB=4,
	LELBOWA=5,
	LELBOWB=6,
	LWRISTA=7,		// mei node 6 axis 0
	LWRISTB=8,		// mei node 6 axis 1
	LTHUMB=9,
	LPAW=10,
	RSHOULDERA=11,
	RSHOULDERB=12,
	RELBOWA=13,
	RELBOWB=14,
	RWRISTA=15,		// mei node 6 axis 2
	RWRISTB=16,		// mei node 6 axis 3
	RTHUMB=17,
	RPAW=18,
	NJOINTS=19
};

enum __gaintypes
{
	PROPORTIONAL=0,
	DERIVATIVE=1,
	INTEGRAL=2,
	INTEGRAL_LIMIT=3,
	NPARAMS=4
};

enum ArmCommandType
{
	COMMAND_NONE = 0,
	COMMAND_SETPOS = 1,
	COMMAND_MOVETO = 2,
	COMMAND_VEL = 3,
	COMMAND_VEL2 = 4
};

// temporary.
const double TorsoDefaultAccelerationValue = 2500.0;

// device names.
#define MEI_NAME0 "//7/dev/mei0x320"
#define MEI_NAME1 "//6/dev/mei0x260"
#define MEI_NAME2 "//3/dev/mei0x320"
#define UEID_NAME "/yarp/uei"

#define DEFAULTARMFILENAME "/ai/ai/mb/cdp/src/YARP/conf/arm_2loops.ini"
#define DEFAULTGRAVITYFILENAME "/ai/ai/mb/cdp/src/YARP/conf/gravity.txt"

// temporary.
inline void __notimplemented() { bool notimplemented = false; assert (notimplemented == true); }

//
class YARPGenericArmControl 
{
private:
	YARPGenericArmControl (const YARPGenericArmControl&);
	void operator= (const YARPGenericArmControl&);

protected:
	static int meidx0;		// meid handle.
	static int meidx1;	
	static int meidx2;
	int ueidx;				// ueid handle.

	static int m_instances;
	static YARPSemaphore m_mutex;		// mutex. 

	inline void Lock(void)	 { m_mutex.Wait (); }
	inline void Unlock(void) { m_mutex.Post (); }

public:
	YARPGenericArmControl ();
	virtual ~YARPGenericArmControl();

protected:
	virtual int Initialize (void);
	virtual int Uninitialize (void);
};

// 
const int __ueid_quantum	= 16;									// quantum of the uei ADC.
const int A2D_CHANNELS		= 32;									// number of channel	
const int BOARD				= 0;									// ueid board no?

const int POSITION_PERIOD	= 6;									// X ms.
const double THREAD_PERIOD  = double(POSITION_PERIOD)/1000.0;		// POSITION_PERIOD/1000 s.

const int MAX_SAMP			= 64;									// ?
const double DEFAULTALPHAVALUE = 0.3;								// LATER: to be tuned.

//
//
//
class __constvel
{
protected:
	int m_time_counter[NJOINTS];

	double m_angle_actual[NJOINTS];
	double m_command_actual[NJOINTS];
	double m_angle_setpoint[NJOINTS];
	double m_speed_setpoint[NJOINTS];

	double m_t1[NJOINTS];
	double m_x0[NJOINTS];
	double m_speed_e[NJOINTS];

	bool m_active[NJOINTS];

public:
	__constvel() 
	{
		memset (m_time_counter, 0, sizeof(int) * NJOINTS);
		memset (m_angle_actual, 0, sizeof(double) * NJOINTS);
		memset (m_command_actual, 0, sizeof(double) * NJOINTS);
		memset (m_angle_setpoint, 0, sizeof(double) * NJOINTS);
		memset (m_speed_setpoint, 0, sizeof(double) * NJOINTS);
		memset (m_active, false, sizeof(bool) * NJOINTS);

		memset (m_t1, 0, sizeof(double) * NJOINTS);
		memset (m_x0, 0, sizeof(double) * NJOINTS);
		memset (m_speed_e, 0, sizeof(double) * NJOINTS);
	}

	~__constvel()
	{
	}

	void ActivateJoint (int jj) { m_active[jj] = true; }
	void DeactivateJoint (int jj) { m_active[jj] = false; }
	bool IsActive (int jj) const { return m_active[jj]; }

	double GetSpeedSetpoint (int jj) const { return m_speed_e[jj]; }
	void ResetTrajectory (int jj) 
	{
		m_speed_e[jj] = 0;
		m_t1[jj] = 0;
	}

	void Set (int jj, double ap, double ac, double sp, double sv)
	{
		m_angle_actual[jj] = ap;
		m_command_actual[jj] = ac;
		m_angle_setpoint[jj] = sp;
		m_speed_setpoint[jj] = sv;
	}

	void InitJoint (int jj)
	{
		m_time_counter[jj] = 0;

		m_t1[jj] = 	fabs(m_angle_actual[jj] - m_angle_setpoint[jj]) / fabs(m_speed_setpoint[jj]);
		m_speed_e[jj] = (m_angle_setpoint[jj] > m_angle_actual[jj]) ? fabs(m_speed_setpoint[jj]) : -fabs(m_speed_setpoint[jj]); 
		m_x0[jj] = m_command_actual[jj];
	}

	bool Step (int jj, double *x)
	{
		if (!m_active[jj])
		{
			return true;
		}

		bool ended = false;
		
		m_time_counter[jj] ++;
		double t = m_time_counter[jj] * THREAD_PERIOD;
		
		if (t <= m_t1[jj])
		{
			*x = m_speed_e[jj] * t + m_x0[jj];
		}
		else
		{
			ended = true;
		}

		return ended;
	}
};

//
// speed_setpoint must be always positive.
// accel_setpoint must be always positive.
// angle_actual, speed_actual, angle_setpoint are signed quantities.
//
class __trapezoidal
{
protected:
	int m_time_counter[NJOINTS];

	double m_angle_actual[NJOINTS];
	double m_speed_actual[NJOINTS];

	double m_angle_setpoint[NJOINTS];
	double m_speed_setpoint[NJOINTS];
	double m_accel_setpoint[NJOINTS];

	double m_x0[NJOINTS];
	double m_x1[NJOINTS];
	double m_x2[NJOINTS];
	double m_x3[NJOINTS];

	double m_speed_e1[NJOINTS];
	double m_accel_e1[NJOINTS];
	double m_speed_e2[NJOINTS];
	double m_accel_e2[NJOINTS];
	double m_speed_e3[NJOINTS];
	double m_accel_e3[NJOINTS];

	double m_t1[NJOINTS];
	double m_t2[NJOINTS];
	double m_t3[NJOINTS];

	bool m_active[NJOINTS];

	double m_old_x[NJOINTS];
	double m_commanded_delta[NJOINTS];

public:
	__trapezoidal (void) 
	{
		memset (m_time_counter, 0, sizeof(int) * NJOINTS);
		
		memset (m_t1, 0, sizeof(double) * NJOINTS);
		memset (m_t2, 0, sizeof(double) * NJOINTS);
		memset (m_t3, 0, sizeof(double) * NJOINTS);

		memset (m_angle_actual, 0, sizeof(double) * NJOINTS);
		memset (m_speed_actual, 0, sizeof(double) * NJOINTS);
		
		memset (m_angle_setpoint, 0, sizeof(double) * NJOINTS);
		memset (m_speed_setpoint, 0, sizeof(double) * NJOINTS);
		memset (m_accel_setpoint, 0, sizeof(double) * NJOINTS);

		memset (m_active, false, sizeof(bool) * NJOINTS);

		memset (m_old_x, 0, sizeof(double) * NJOINTS);
		memset (m_commanded_delta, 0, sizeof(double) * NJOINTS);
	}

	~__trapezoidal () {}

	void ActivateJoint (int jj) { m_active[jj] = true; }
	void DeactivateJoint (int jj) { m_active[jj] = false; }
	bool IsActive (int jj) const { return m_active[jj]; }

	void SetAngleActual (double *v) { memcpy (m_angle_actual, v, sizeof(double) * NJOINTS);	}
	void SetSpeedActual (double *v) { memcpy (m_speed_actual, v, sizeof(double) * NJOINTS); }
	
	void SetAngleSetpoint (double *v) { memcpy (m_angle_setpoint, v, sizeof(double) * NJOINTS); }
	void SetSpeedSetpoint (double *v) { memcpy (m_speed_setpoint, v, sizeof(double) * NJOINTS); }
	void SetAccelSetpoint (double *v) { memcpy (m_accel_setpoint, v, sizeof(double) * NJOINTS); }

	double GetSpeedSetpoint (int jj) const { return m_commanded_delta[jj]; }

	// if they do not need to be changed it might be convenient.
	void SetAllParams (double *sv, double *sa)
	{
		memcpy (m_speed_setpoint, sv, sizeof(double) * NJOINTS);
		memcpy (m_accel_setpoint, sa, sizeof(double) * NJOINTS);
	}

	void Set (int jj, double ap, double av, double sp, double sv, double sa)
	{
		m_angle_actual[jj] = ap;
		m_speed_actual[jj] = av;
		m_angle_setpoint[jj] = sp;
		m_speed_setpoint[jj] = sv;
		m_accel_setpoint[jj] = sa;
	}

	void ResetTrajectory (int jj)
	{
		m_commanded_delta[jj] = 0;
	}

	void InitJoint (int jj)
	{
		m_time_counter[jj] = 0;

		// real parameters for interpolation.
		m_speed_e2[jj] = (m_angle_setpoint[jj] < m_angle_actual[jj]) ? -fabs(m_speed_setpoint[jj]) : fabs(m_speed_setpoint[jj]);
		m_accel_e2[jj] = 0;

		m_speed_e1[jj] = m_speed_actual[jj];
		m_accel_e1[jj] = (m_speed_e2[jj] < m_speed_actual[jj]) ? -fabs(m_accel_setpoint[jj]) : fabs(m_accel_setpoint[jj]);
		
		m_speed_e3[jj] = m_speed_e2[jj];
		m_accel_e3[jj] = (m_speed_e2[jj] < 0.0) ? -fabs(m_accel_setpoint[jj]) : fabs(m_accel_setpoint[jj]); 

		// t1,t2,t3 are deltas.
		double dt1 = m_t1[jj] = fabs(m_speed_e2[jj] - m_speed_actual[jj]) / fabs(m_accel_setpoint[jj]);
		double dx1 = fabs( .5 * m_accel_e1[jj] * dt1 * dt1 + m_speed_e1[jj] * dt1 );
		
		double dt3 = m_t3[jj] = fabs(m_speed_setpoint[jj])/fabs(m_accel_setpoint[jj]);
		double dx3 = fabs( -.5 * m_accel_e3[jj] * dt3 * dt3 + m_speed_e3[jj] * dt3 );

		double dt2 = m_t2[jj] = (fabs(m_angle_setpoint[jj] - m_angle_actual[jj]) - dx1 - dx3) / fabs(m_speed_setpoint[jj]);

		// x1,x2.
		m_x0[jj] = m_angle_actual[jj];
		m_x1[jj] = .5 * m_accel_e1[jj] * dt1 * dt1 + m_speed_e1[jj] * dt1 + m_x0[jj];
		m_x2[jj] = m_speed_e2[jj] * dt2 + m_x1[jj];
		m_x3[jj] = m_angle_setpoint[jj];

		m_old_x[jj] = m_x0[jj];
		m_commanded_delta[jj] = 0;

		if (dt2 < 0)
		{
			// triangular trajectory. not enough time to do the trapezoidal.
			//printf ("__trapezoidal: generating triangular\n");

			// the max achievable speed.
			double vmax = sqrt (m_accel_e1[jj]*(m_angle_setpoint[jj]-m_angle_actual[jj]) + m_speed_actual[jj]*m_speed_actual[jj]/2.0);
			if (m_speed_e2[jj] < 0)
				vmax = -vmax;

			//printf ("vmax: %lf\n", vmax);

			// no e2 params. store vmax.
			m_speed_e2[jj] = vmax;

			// speed and accel e1 fine.
			dt1 = m_t1[jj] = (vmax - m_speed_e1[jj]) / m_accel_e1[jj];

			// e3.
			dt3 = m_t3[jj] = vmax / m_accel_e1[jj];
			m_speed_e3[jj] = vmax;
			//m_accel_e3[jj] = no change.

			// m_x0[jj] the same.
			m_x1[jj] = .5 * m_accel_e1[jj] * dt1 * dt1 + m_speed_e1[jj] * dt1 + m_x0[jj];
			m_x2[jj] = 0;	// unused.
			// m_x3[jj] = m_angle_setpoint[jj]; same.
		}
	}

	// returns whether the trajectory ended.
	bool Step (int jj, double *x)
	{
		if (!m_active[jj])
		{
			m_commanded_delta[jj] = 0;
			return true;
		}

		bool ended = false;
		
		if (m_t2[jj] >= 0)
		{
			m_time_counter[jj] ++;
			double t = m_time_counter[jj] * THREAD_PERIOD;
			
			if (t < m_t1[jj])
			{
				*x = .5 * m_accel_e1[jj] * t * t + m_speed_e1[jj] * t + m_x0[jj];
			}
			else
			if (t < m_t1[jj]+m_t2[jj])
			{
				*x = m_speed_e2[jj] * (t - m_t1[jj]) + m_x1[jj];
			}
			else
			if (t <= m_t1[jj]+m_t2[jj]+m_t3[jj])
			{
				double t2 = m_t2[jj] + m_t1[jj];
				*x = -.5 * m_accel_e3[jj] * (t - t2) * (t - t2) + m_speed_e3[jj] * (t - t2) + m_x2[jj];
			}
			else
			{
				// last point.
				*x = m_x3[jj];
				m_old_x[jj] = *x;
				ended = true;
			}
		}
		else
		{
			// replace with the triangular trajectory generation.
			m_time_counter[jj] ++;
			double t = m_time_counter[jj] * THREAD_PERIOD;
		
			if (t < m_t1[jj])
			{
				*x = .5 * m_accel_e1[jj] * t * t + m_speed_e1[jj] * t + m_x0[jj];
			}
			else
			if (t < m_t1[jj] + m_t3[jj])
			{
				double t1 = m_t1[jj];
				*x = -.5 * m_accel_e3[jj] * (t-t1) * (t-t1) + m_speed_e3[jj] * (t-t1) + m_x1[jj]; 
			}
			else
			{
				*x = m_x3[jj];
				m_old_x[jj] = *x;
				ended = true;
			}
		}

		m_commanded_delta[jj] = (*x - m_old_x[jj]) / THREAD_PERIOD;
		m_old_x[jj] = *x;

		return ended;
	}
};


class __gravitycomp
{
protected:
	__CGTable m_table;

	enum { ARRAYSTEP = 5 };

	double m_min, m_max;

	int m_counter;
	double m_old_angle[NJOINTS];

	bool m_active;
	bool m_learning;
	bool m_loaded;

	CVisDVector x, y;

public:
	__gravitycomp ()
	{
		m_min = m_max = 0;
		m_counter = 0;
		
		memset (m_old_angle, 0, sizeof(double) * NJOINTS);

		m_active = true;
		m_loaded = false;
		m_learning = false;
	}

	~__gravitycomp () 
	{
	}

	void ToggleActive (void) { m_active = !m_active; }
	void ToggleLearning (void) { m_learning = !m_learning; }

	bool IsActive (void) const { return m_active; }
	bool IsLoaded (void) const { return m_loaded; }
	bool IsLearning (void) const { return m_learning; }

	void Init (double inf, double sup)
	{
		m_table.Rebuild (1, 1, 5.0, 0.0);
		x.Resize(1);
		y.Resize(1);

		x = 0;
		y = 0;
		m_table.AddNew (x, y);

		m_min = inf;
		m_max = sup;

		printf ("__gravitycomp: init\n");
		printf ("min %lf max %lf\n", m_min, m_max);
	}

	bool Add (double position, double value)
	{
		if (!m_learning)
			return false;

		if (position < m_min || position > m_max)
			return false;

		x(1) = position;
		y(1) = value;
		m_table.Nearest(x);
		m_table.CheckAndAdd (x, y);

		printf ("__gravitycomp: added %lf -> %lf\n", position, value);
		return true;
	}

	void SaveToFile (char *filename)
	{
		m_table.DumpToFile (filename);
	}

	void LoadFromFile (char *filename)
	{
		if (!m_table.LoadFromFile (filename))
		{
			printf ("__gravitycomp: can't load map from file %s\n", filename);
			m_loaded = false;
		}

		m_loaded = true;
	}

	// DEBUG ALL THIS. TUNE THE THRESHOLD.
	bool StandingStable (double *angle)
	{
		const double THRESHOLD = 4;
		const int COUNTER_THR = 300;	// LATER: this must be higher.
		bool standing = true;

		for (int i = 3; i <= 8; i++)
		{
			standing &= (fabs(angle[i] - m_old_angle[i]) < THRESHOLD); 
		}

		memcpy (m_old_angle, angle, sizeof(double) * NJOINTS);

		if (!standing)
			m_counter = 0;

		m_counter ++;
		if (m_counter > COUNTER_THR)
		{
			m_counter = 0;
			return true;
		}

		return false;
	}

	double Nearest (double xx)
	{
		x(1) = xx;
		return m_table.Nearest (x)(1);
	}
};

// I'm kind of loosing control of this class, too many options and different modes.

/*
	double **m_gains;				positional gains. for controllers by the control thread
	double **m_2b_gains;			double buffer of m_gains
	double **m_gains_hold;			a copy of the gain(s) to be used as setpoint when raising/decreasing
	bool m_zero_gain;				whether the gains are zero

	uint16_t u_pot_vals[MAX_SAMP];	raw values from the pots
	int *m_pot_zero;				zero of the potentiometers
	int *m_torque_zero;				zero of the torque (strain gauges)

	double *m_limits_min;			limits of angular variables
	double *m_limits_max;			"

	bool *m_enabled;				enabled flag
	bool *m_calibrated;				calibrated flag (position, mostly fake because there's no p calib)
	bool *m_t_calibrated;			torque calibration flag, this is real
	bool m_enabletorque;			enable reading torque values back from the A/D (MEI)

	bool *m_intrajectory;			whether a trajectory is being computed (either velocity or trapezoidal)

	double *m_angle;				angular position
	double *m_2b_angle;				double buffer of m_angle
	double *m_command;				commanded position, not used for the torso
	double *m_2b_command;			double buffer of m_command, copied back when trajectory is completed
	double *m_speed;				speed of velocity command (also for the torso)
	double *m_2b_speed;				double buffer of m_speed (NOT THE actual speed)

	int16 m_16bit_oldangle[3];		to take into account that encoders are 16 bits only
	short m_winding[3];				how many times there's been an overflow

	double *m_error;				error of the PID when computed by the thread (not the MEI)
	double *m_derror;				derivative of the error of the PID
	double *m_ierror;				integral of the error of the PID
	double *m_old_error;			temporary for computing the derivative

	double *m_speed_setpoint;		setpoint for trapezoidal trajectory generation
	double *m_accel_setpoint;		setpoint for trapezoidal but also used for the torso when in vmode

	int16 *m_torque;				torque values as read from the MEI (ADC)
	int16 *m_old_torque;			old values for computing a PD controller of torque
	int16 *m_2b_torque;				double buffer of m_torque for exporting the values

	ArmCommandType *m_lastcommand;	last command (position, velocity, none) used to decide how to update m_command

	double m_alpha;					filter cutoff freq for smoothing out the derivative of the PID (not the torso)
	double *m_oldfiltervalues;		filter temporary variable

	// mostly for debugging.
	clock_t m_prev_cycle;			time value of the previous thread cycle
	bool m_timing;					whether timing is on or off
*/

class YARPArmControl;

//
// we might need to raise the priority to guarantee time constraints.
// it's mandatory to run as root. Thread won't start if can't get high priority.
class __positioncontrol : public YARPRateThread
{
protected:
	int ueidx;	// local ueid handle.
	
	double **m_gains;
	double **m_2b_gains;	// protected memory.
	double **m_gains_hold;	// keeps a copy of the actual gains.
	bool m_zero_gain;

	uint16_t u_pot_vals[MAX_SAMP];
	int *m_pot_zero;
	int *m_torque_zero;

	double *m_limits_min;
	double *m_limits_max;

	bool *m_enabled;
	bool *m_calibrated;
	bool *m_t_calibrated;
	bool m_enabletorque;

	bool *m_intrajectory;

	double *m_angle;
	double *m_2b_angle;
	double *m_command;
	double *m_2b_command;
	double *m_speed;
	double *m_2b_speed;

	// this is for the torso (axis 0,1,2).
	int16 m_16bit_oldangle[3];
	short m_winding[3];
	// reuse pot zero for the encoder zero (it's integer, it's fine).

	double *m_error;
	double *m_2b_error;

	double *m_derror;
	double *m_ierror;
	double *m_old_error;

	double *m_speed_setpoint;
	double *m_accel_setpoint;

	int16 *m_torque;
	int16 *m_old_torque;
	int16 *m_2b_torque;

	ArmCommandType *m_lastcommand;

	double m_alpha;
	double *m_oldfiltervalues;

	// mostly for debugging.
	clock_t m_prev_cycle;
	bool m_timing;

	__trapezoidal m_trapezoidal;	// trapezoidal vel profile
	__constvel m_constvel;			// const vel profile
	__gravitycomp m_gravity;

	inline void ComputeErrors (void);
	inline void ComputeTrajectory (void);
	inline void ComputeController (YARPArmControl& control);
	inline void ReadPositions (YARPArmControl& control);

public:
	__positioncontrol ();
	virtual ~__positioncontrol ();

	virtual void DoInit(void *caller);
	virtual void DoLoop(void *caller);
	virtual void DoRelease(void *caller);

	void SetPositionGain (int joint, int param, double value);
	void SetPositionGains (int joint, double *values);
	void GetPositionGain (int joint, int param, double *value);
	void GetPositionGains (int joint, double *values);
	bool GetGainStatus (void);	// true means gains are UP!

	void RaiseGains (void);
	void LowerGains (void);

	void SetPosition (int joint, double position);
	void GetPosition (int joint, double *position);
	void GetPositions (double *positions);
	void SetPositions (double *positions);
	void SetPCalibratedFlag (int joint);
	void SetPositionLimits (int joint, double min, double max);
	void GetPositionLimits (int joint, double *min, double *max);
	void SetSpeedSetpoint (int joint, double v);
	void SetAccelSetpoint (int joint, double v);
	void MoveTo (int joint, double value);
	void MoveTo (int howmany, int *joints, double *values);
	void MoveToSimple (int joint, double value);
	void VMove (int joint, double value);
	void VMove (double *values);
	void VMove (int joint1, int joint2, double *values);

	void GetError (int joint, double* err);
	void GetErrors (double *err);

	void EnableAxis (int joint);
	void DisableAxis (int joint);

	void EnableTorque (void);
	void DisableTorque (void);
	void ToggleTorque (void);
	void GetTorque (int joint, double *torque);
	void SetTorque (int joint, double torque) { __notimplemented(); }
	void GetTorques (double *torques);
	void SetTorques (double *torques) { __notimplemented(); }

	void SetTCalibratedFlag (int joint, int zero);
	bool GetTCalibratedFlag (int joint);

	void SetAlphaFilter (double alpha);
	void GetAlphaFilter (double *alpha);

	void ToggleTiming (void) { m_timing = !m_timing; }
	
	// Gravity compensation (only for joint 3).
	void GravitySaveToFile (void);
	void GravityLoadFromFile (void);
	void GravityToggleActive (void); 
	void GravityToggleLearning (void); 
};


//
//
// New functionalities should be maintained in this class.
//
// TODO: position calibration (using hard limits).
//		 methods to set and read torque values.
//		 velocity control when in position mode.
//		 trajectory generator when in position mode.
//		 check ueid initialization.
//
// LATER: we might have a new mode where the thread is not necessarily run
//	and torque commands are directly issued to the MEI card (for hacking).
// LATER: hand/torso stuff might need a special calibration.
//
//
class YARPArmControl : public YARPGenericArmControl
{
protected:
	friend class __positioncontrol;		// simplified access.
	static __positioncontrol m_thread;

	// I ended up splitting the parameters between the __positioncontrol
	//	and the YARPArmControl.
	// LATER: move everything to the thread class which eventually uses
	//	the params.
	static int16 **m_control_params;	// NJOINTS * COEFFICIENTS (MEI)
	static int *m_pot_zero;				// initial reading of the pots.
	static int *m_torque_zero;			// zero of the strain gauges.
	static bool m_torso_shutdown_flag;	// true if the gains are 0.

	inline int get_uei_info (int axis) const 
		{ return get_uei_info((__jointtypes)axis); }
	inline int get_uei_info (__jointtypes axis) const;

	inline int get_mei_info(__jointtypes axis, int * meidx) const;
	inline int get_mei_info(int axis, int * meidx) const 
		{ return get_mei_info((__jointtypes)axis, meidx); }
	
	inline void ReadZeroPositions (void);

	void ReadConfig (const char *filename);
	void EatLine (FILE *fp);

public:
	YARPArmControl ();
	virtual ~YARPArmControl ();

	int Initialize (void);
	int Initialize (const char *filename);
	int Uninitialize (void);
	
	int StartPositionControl (bool wait) { m_thread.Start(wait); return 0; }
	int StopPositionControl (bool wait) { m_thread.Terminate(wait); return 0; }

	void SetPositionGain (int joint, int param, double value) { m_thread.SetPositionGain (joint, param, value); }
	void GetPositionGain (int joint, int param, double *value) { m_thread.GetPositionGain (joint, param, value); }
	
	void SetMEIGain (int joint, int param, int16 value);
	void GetMEIGain (int joint, int param, int16 *value);
	void SetMEIGains (int joint, int16 *value);
	void GetMEIGains (int joint, int16 *value);

	// for DEBUG.
	void PrintAxis (int joint);

	void RaisePositionGains (void) { m_thread.RaiseGains(); }
	void LowerPositionGains (void) { m_thread.LowerGains(); }
	bool GetGainStatus (void) { return m_thread.GetGainStatus(); }

	// these are implemented here.
	void RaiseMEIGains (void);
	void LowerMEIGains (void);
	bool GetMEIGainStatus (void);

	void EnableAxis (int joint) { m_thread.EnableAxis(joint); }
	void DisableAxis (int joint) { m_thread.DisableAxis(joint); }

	// we might insert here the encoder to angle conversion.
	void SetPosition (int joint, double position) { m_thread.SetPosition (joint, position); }
	void SetPositions (double *positions) { m_thread.SetPositions (positions); }
	void GetPosition (int joint, double *position) { m_thread.GetPosition (joint, position); }
	void GetPositions (double *positions) { m_thread.GetPositions (positions); }
	void GetPositionZero (int joint, double *zero) 
	{
		if (joint < 0 || joint >= NJOINTS)
		{
			*zero = 0;
			return;
		}
		Lock ();
		*zero = double(m_pot_zero[joint]); 
		Unlock ();
	}
	void GetPositionZeros (double *zero) 
	{
		Lock ();
		for (int i = 0; i < NJOINTS; i++)
			zero[i] = double(m_pot_zero[i]);
		Unlock ();
	}
	void MoveTo (int joint, double position) { m_thread.MoveTo (joint, position); }
	void MoveTo (int howmany, int *joints, double *positions) { m_thread.MoveTo (howmany, joints, positions); }
	void MoveToSimple (int joint, double position) { m_thread.MoveToSimple (joint, position); }
	void SetSpeedSetpoint (int joint, double v) { m_thread.SetSpeedSetpoint (joint, v); }
	void SetAccelSetpoint (int joint, double v) { m_thread.SetAccelSetpoint (joint, v); }
	void VMove (int joint, double value) { m_thread.VMove (joint, value); }
	void VMove (double *values) { m_thread.VMove (values); }
	void VMove (int joint1, int joint2, double *values) { m_thread.VMove(joint1, joint2, values); }

	void TrustDefaultLimits (int joint) { m_thread.SetPCalibratedFlag (joint); }

	void GetError (int j, double *err) { m_thread.GetError (j, err); }
	void GetErrors (double *err) { m_thread.GetErrors (err); }

	void EnableTorque(void) { m_thread.EnableTorque(); }
	void DisableTorque(void) { m_thread.DisableTorque(); }
	void ToggleTorque(void) { m_thread.ToggleTorque(); }
	void GetTorque (int joint, double *torque) { m_thread.GetTorque (joint, torque); }
	void GetTorques (double *torques) { m_thread.GetTorques (torques); }

	void SetTorque (int joint, double torque) { __notimplemented(); }
	void SetTorques (double *torques) { __notimplemented(); }
	void TorqueCalibration (int joint);
	void TorqueCalibration (void);

	void SetAlphaFilter (double a) { m_thread.SetAlphaFilter(a); }
	void GetAlphaFilter (double *a) { m_thread.GetAlphaFilter(a); }
	
	inline int GetNJoints(void) const { return NJOINTS; }
	inline void ToggleTiming (void) { m_thread.ToggleTiming(); }

	inline void GetLimits (int jnt, double *min, double *max) {	m_thread.GetPositionLimits (jnt, min, max); }
	inline bool GetTCalibratedFlag (int jnt) { return m_thread.GetTCalibratedFlag (jnt); }

	inline void GravitySaveToFile(void) { m_thread.GravitySaveToFile(); }
	inline void GravityLoadFromFile(void) { m_thread.GravityLoadFromFile(); }
	inline void GravityToggleActive(void) { m_thread.GravityToggleActive(); }
	inline void GravityToggleLearning(void) { m_thread.GravityToggleLearning(); }
};

#include "ArmControl.inl"

#endif