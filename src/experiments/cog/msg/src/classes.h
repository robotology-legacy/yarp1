//
// reaching threads.
//	- Halloween 2001.
//  - April 2002.
//  - May 2002.
//	- Jun 2002.
//

#ifndef __classesh__
#define __classesh__

#include <sys/kernel.h>
#include <iostream.h>
#include <time.h>

#include "YARPPort.h"
#include "YARPThread.h"
#include "YARPSemaphore.h"
#include "YARPTime.h"
#include "YARPRandomNumber.h"
#include "CogGaze.h"
#include "YARPBottle.h"

#include <VisMatrix.h>
#include <Models.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

#include "YARPSmallStateMachine.h"
#include "YARPPlanarReaching.h"

#include "portnames.h"

#define CALIBRATIONFILENAME "/ai/ai/mb/cdp/src/YARP/conf/calibration.txt"
 
//
// LATER: velocity is not computed correctly while switching between modes.
//	it might be important to fix this although it's not causing any misbehavior.
// The same consideration applies to the counters, relax behavior, etc.
//

//
// LATER: move inside respective classes.
// 
const int NPRIMITIVES = 3;
const double primitives[NPRIMITIVES][6]=
{
	// sort of zeros.
	// 294, 240, 415, 704, 680, 541
	// removed 20 from the last 2 primitives.
	// tweaked joint 7(5) to allow better waving of the arm.
	{ 294.0, 250.0, 415.0, 704.0, 490.0, 541.0 },
	{ 364.0, 279.0, 465.0, 544.0, 490.0, 541.0 },
	{ 380.0, 255.0, 295.0, 514.0, 480.0, 541.0 }
};

const double primitives_planar[NPRIMITIVES][6]=
{
	// sort of zeros.
	// 294, 240, 415, 704, 680, 541
	// removed 20 from the last 2 primitives.
	{ 294.0, 250.0, 415.0, 704.0, 590.0, 541.0 },
	{ 364.0, 279.0, 465.0, 544.0, 590.0, 650.0 },
	{ 380.0, 255.0, 295.0, 514.0, 580.0, 641.0 }
};

const double chestposition[6] =
{
	424.0, 240.0, 275.0, 480.0, 580.0, 641.0
};

const double intermediate[4][2] =
{
	{ 0.0, 0.0 },
	{ -10.0, 0.0 },
	{ 0.0, 0.0 },
	{ 0.0, 0.0 }
};

// use actual direction (from egomap) istead of this!
const double input_prim[NPRIMITIVES][2]=
{
	{ 0.0, 0.0 },
	{ -30.0, 60.0 },
	{ 0.0, 60.0 },
};

const double output_prim[NPRIMITIVES][NPRIMITIVES]=
{
	{ 1.0, 0.0, 0.0 },
	{ 0.0, 1.0, 0.0 },
	{ 0.0, 0.0, 1.0 }
};


class ReachingThread;

// Mode 1. Do nothing.
// accepts messages to be delivered to the head/arm controller.
//
template <class CALLER>
class IdleMode : public SmallStateMachine<IdleMode, YARPMTStateMachine, CALLER>
{
protected:
	enum { MAXNUMMESSAGES = 10 }; 
	HeadMessage m_hmsg[MAXNUMMESSAGES];
	int m_nhmsg;
	ArmMessage  m_amsg[MAXNUMMESSAGES];
	int m_namsg;

	virtual void Common (CALLER *) {}

	void ReachNull (CALLER *caller);
	void ReachMixedMessage (CALLER *caller);
	void ReachHeadMessage (CALLER *caller);
	void ReachArmMessage (CALLER *caller);

public:
	IdleMode ();
	void ResetState (void);
 	void SendArmMessage (ArmMessage *msg);
	void SendHeadMessage (HeadMessage *msg);
	void SendMessageSequence (ArmMessage *amsg, int namsg, HeadMessage *hmsg, int nhmsg);
};

//
// Mode 2
// Standard reach mode (sort of general purpose).
//
template <class CALLER>
class StdReachingMode : 
	public SmallStateMachine<StdReachingMode, YARPMTStateMachine, CALLER>
{
protected:
	enum { REACHLENGTH = 20, RELAXLENGTH = 30 };

	CLookupTable m_lut;
	clock_t m_now;
	clock_t m_prev_cycle;
	int m_last_cycle;

	double m_tin[2];
	double m_tout[NPRIMITIVES];
	double m_tjnt[6];
	double m_noiselevel;

	int m_count_reaching;
	int m_count_fatique;
	JointPos m_old_pos;
	JointPos m_speed;
	JointPos m_pos;

	ArmJoints m_positionarm;

	bool m_wave;

	HeadMessage m_hmsg;
	ArmMessage  m_amsg;

	CogGaze m_gaze;
	CVisDVector m_curg;

	void ForwardKin (CVisDVector& out); 
	virtual void Common (CALLER *caller);

	void ReachNull (CALLER *);
	void ReachIdle (CALLER *);
	void ReachInit (CALLER *caller);
	void ReachMove (CALLER *caller);
	void ReachWave (CALLER *caller);
	void ReachRelax (CALLER *caller);

public:
	StdReachingMode ();

	void ResetState (void);
	void ToggleWaveArm (void);
	void SetNoiseLevel (double noise);
};

//
//
// LATER: Replace middle coordinate system by the right eye coordinate system.
//	// it can be more esily checked on the images.
//
//

#define THRESHOLD 0.5

class PreciseFixationFake 
{
public:
	CogGaze m_gaze;

	void ComputeCorrection (const JointPos& head, const ArmJoints& arm, double x, double y, float& theta, float& phi)
	{
		m_gaze.Apply ((JointPos&)head, (ArmJoints&)arm);
		m_gaze.DeIntersect (float(x), float(y), theta, phi);

		printf ("DeIntersect: %f %f, returned %f %f\n", float(x), float(y), theta, phi);
		printf ("Current gaze is: %f %f\n", m_gaze.theta_right, m_gaze.phi_right);
	}

	void ComputeDirection (const JointPos& head, const ArmJoints& arm, float& theta, float& phi)
	{
		m_gaze.Apply ((JointPos&)head, (ArmJoints&)arm);
		theta = m_gaze.theta_middle;
		phi = m_gaze.phi_middle;
	}
};

//
// Mode 3.
// reaching 2D on a table, open loop.
//
template <class CALLER>
class PlanarMode : 
	public SmallStateMachine<PlanarMode, YARPMTStateMachine, CALLER>
{
protected:
	enum 
	{ 
		BASISV = 3, 
		SIDEPOS = 2 /* 10 */, 
		REACHLENGTH = 4 /* 10 */, 
		RELAXLENGTH = 15, 
		FATIQUE = 1000 
	};
	enum 
	{ 
		INSIZE = 2, 
		ARMSIZE = 6 
	};

	clock_t m_now;
	clock_t m_prev_cycle;
	int m_last_cycle;

	int m_count_reaching;
	int m_count_fatique;
	int m_count_sidepos;

	JointPos m_old_pos;
	JointPos m_speed;
	JointPos m_pos;
	StereoPosData m_image_target;
	StereoPosData m_store_image_target;

	GyroPos m_gyro;
	CogGaze m_gaze;
	
	ArmJoints m_positionarm;

	HeadMessage m_hmsg;
	ArmMessage  m_amsg;

	bool m_signal;
	bool m_wave_arm;
	bool m_calibrated;
	bool m_last_commanded;
	int m_initial_position;
	int m_signal_counter;
	
	YARPPlanarReaching m_reaching;

	// LATER:
	PreciseFixationFake m_fixation_helper;

	// communicate with higher level control
	float m_torso;
	int m_action_no;

	CVisDVector m_curg;
	CVisDVector m_control;
	CVisDVector m_commanded;

	PFVOID m_storestate;

	int m_moving_counter;
	int m_fixx, m_fixy;
	int m_newfixationpoint;

	bool NotMovingHead (void);
	void ClipControl (CVisDVector& c);
	bool CloseEnough (void);

	virtual void Common (CALLER *caller);

	void ReachNull (CALLER *);
	void ReachIdle (CALLER *caller);
	void ReachInit (CALLER *caller);
	void ReachMove (CALLER *caller);
	void ReachRelax (CALLER *caller);
	void ReachHeadMessage (CALLER *caller);
	void ReachArmMessage (CALLER *caller);
	void ReachSideposition (CALLER *caller);
	void ReachSideposition2 (CALLER *caller);
	void ReachWait (CALLER *caller);
	void ReachSidepositionPrepare (CALLER *caller);
	void RecordingOnly (CALLER *caller);

	void ReachInit2 (CALLER *caller);
	void ReachMove2 (CALLER *caller);

public:
	PlanarMode ();

	void ResetState (void);

	void LoadCalibration (const char *filename);
	void BuildTemporaryVectors (void);
	void Uncalibrate (void);
	void StoreCalibration (const char *filename);
	void StoreSidePosition (void);
	void StoreBasisPosition (int ii);
	void SendArmMessage (ArmMessage *msg);
	void SendHeadMessage (HeadMessage *msg);

	void SetPreciseFixationPnt (int x, int y);

	inline void ToggleWaveArm (void) { Lock(); m_wave_arm = !m_wave_arm; Unlock(); }
};


//
// Modes are:
//	- 0 not used.
//	- 1 idle.
//	- 2 std reaching.
//  - 3 reaching on the table.
//

class HighLevelPort : public YARPOutputPortOf<YARPBottle>
{
public:
	HighLevelPort (void) : YARPOutputPortOf<YARPBottle>() {}

	void SendMsg (NetInt32 v);
	void SendMsg (NetInt32 v, float x1, float x2);
	void SendMsg (NetInt32 v, int x1);
};

class HighLevelPortReply : public YARPInputPortOf<YARPBottle>
{
protected:
	YARPBottle m_bottle;
	double m_timeout;
	YARPSemaphore m_mutex;

public:
	HighLevelPortReply (void) 
		: m_mutex(1), 
		  YARPInputPortOf<YARPBottle>(YARPInputPortOf::DOUBLE_BUFFERS) 
	{ 
		m_timeout = 0; 
		YARPBottleIterator it(m_bottle);
		it.Reset ();
		it.WriteVocab(YBC_SHLC_NONE);
	}

	inline void SetTimeout (double timeout) { m_timeout = timeout; }

	virtual void OnRead(void);
	void GetMsg (YARPBottle& b);
	int GetMsgBlock (NetInt32 v, float& x1, int& x2, double timeout = -1.0);
	int GetMsgBlock (NetInt32 v, int& x1, double timeout = -1.0);
	int GetMsgBlock (NetInt32 v, double timeout = -1.0);
};

//
//
//
//
//
//
class ReachingThread : public YARPThread 
{
	friend class InputBehavior;

public:
	// ports and friends.
	// received back from control level.
	YARPInputPortOf<JointPos>		m_headpos;
	YARPInputPortOf<ArmJoints>		m_armpos;
	YARPInputPortOf<StereoPosData>	m_targetpos;

	// these are to talk to control level.
	YARPOutputPortOf<HeadMessage>   m_ohead;
	YARPOutputPortOf<ArmMessage>    m_oarm;

	HighLevelPort					m_msg_port;
	HighLevelPortReply				m_ans_port;

protected:

	//
	// Modes.
	IdleMode<ReachingThread>			m_mode_idle;
	StdReachingMode<ReachingThread>		m_mode_stdreach;
	PlanarMode<ReachingThread>			m_mode_planar;

	int m_mode;
	YARPSemaphore m_sema;

	inline void Lock () { m_sema.Wait(); }
	inline void Unlock () { m_sema.Post(); }

	bool m_active3;
	bool m_mode3debug;

public:
	ReachingThread () : 
	  YARPThread(), 
	  m_sema(1), 
	  m_msg_port(),
	  m_ans_port()
	{
		  m_ans_port.SetTimeout (0.3);
	}

	virtual void Body ();

	inline void Start (bool s = true) { Begin (); }
	inline void Terminate (bool s = true) { End (); }

	// GUI...
	// std reaching.
	inline void ToggleWaveArm (void) { m_mode_stdreach.ToggleWaveArm (); }
	inline void SetNoiseLevel (double noise) { m_mode_stdreach.SetNoiseLevel (noise); }

	// idle mode (and other stuff).
	inline void SendArmMessage (ArmMessage *msg) 
	{ 
		if (m_mode == 1) 
			m_mode_idle.SendArmMessage (msg); 
		else
		if (m_mode == 3)
			m_mode_planar.SendArmMessage (msg);
	}

	inline void SendHeadMessage (HeadMessage *msg) 
	{ 
		if (m_mode == 1) 
			m_mode_idle.SendHeadMessage (msg); 
		else
		if (m_mode == 3)
			m_mode_planar.SendHeadMessage (msg);
	}

	inline void SendMessageSequence (ArmMessage *amsg, int namsg, 
									 HeadMessage *hmsg, int nhmsg)
	{
		if (m_mode == 1) m_mode_idle.SendMessageSequence (amsg, namsg, hmsg, nhmsg);
	}

	// planar reaching.
	inline void StoreSidePosition (void) 
	{ 
		if (m_mode == 3) m_mode_planar.StoreSidePosition(); 
	}
	inline void StoreBasisPosition (int ii) 
	{ 
		if (m_mode == 3) m_mode_planar.StoreBasisPosition(ii); 
	}
	inline void StoreCalibration (void) 
	{ 
		if (m_mode == 3) m_mode_planar.StoreCalibration (CALIBRATIONFILENAME); 
	}
	inline void LoadCalibration (void) 
	{ 
		if (m_mode == 3) m_mode_planar.LoadCalibration (CALIBRATIONFILENAME); 
	}
	inline void BuildTemporaryVectors (void) 
	{ 
		if (m_mode == 3) m_mode_planar.BuildTemporaryVectors(); 
	}
	inline void Uncalibrate (void)
	{
		if (m_mode == 3) m_mode_planar.Uncalibrate();
	}

	inline void ToggleWaveArm3 (void) { m_mode_planar.ToggleWaveArm(); }

	void StartReaching ();
	void StopReaching ();
	void StartPlanar ();
	void Mode3(); 
	void ToggleDebugMode3();
};


#include "classes.inl"


#endif