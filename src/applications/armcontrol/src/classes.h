//
// classes.h
//

#ifndef __classesarmcontrolh__
#define __classesarmcontrolh__

#include "Models.h"
#include "conf/tx_data.h"

const int encoder_ms = 20;			// this was 10ms
const int torso_control_ms = 30;	// this was 20ms

const double torso_a_default = 10000.0;
const double torso_maxvel = 1000.0;
const double torso_deadband = 0.0;
const double torso_prop = 0.08;

const double torso_position_prop = torso_prop * 10;

const int priority_torso = 28;
const int priority_encoder = 27;

//
//
//
// LATER: extend control to the other 2 axes of the torso.
//	controls only yaw.
class CTorsoController : public YARPRateThread
{
protected:
	YARPArmControl m_arm;

	YARPInputPortOf <JointPos> m_head_position_channel;
	JointPos m_headp;

	CVisDVector m_arm_position;
	CVisDVector m_head_position;

	CPdFilter m_pid;
	CPdFilter m_pid_position;

	double m_error;
	int m_counter;
	double m_control;
	double m_desired;

	bool m_stopped;
	bool m_position_control;

	void DoInit (void *)
	{
		if (m_arm.Initialize () < 0)
			printf ("torso controller thread: can't initialize arm\n");

		m_arm_position = 0;
		m_head_position = 0;

		m_pid.setKs (torso_prop, 0.0);
		m_pid_position.setKs (torso_position_prop, 0.0);

		m_error = 0;
		m_counter = 0;
		m_control = 0;
		m_desired = 0;

		m_stopped = false;
		m_position_control = false;
		
		// set accel.
		m_arm.SetAccelSetpoint (2, torso_a_default);
	}

	void DoLoop (void *)
	{
		Lock ();
		if (!m_position_control)
		{
			if (m_head_position_channel.Read (0))
			{					
				// new data available.
				m_headp = m_head_position_channel.Content();
				
				// compute control.
				m_error = -m_headp(6);
				m_control = m_pid.pd (m_error);
				m_counter = 0;
			}
			else
			{
				// start counting and stop if lost connection?
				m_counter ++;

				if (m_counter >= 5)
				{
					m_control = 0;
				}
			}
		}
		else
		{
			double curpos = 0;
			m_arm.GetPosition (2, &curpos);

			m_error = (m_desired - curpos); 
			m_control = m_pid_position.pd (m_error);
		}
		Unlock ();

		if (m_control < -torso_maxvel)
			m_control = -torso_maxvel;
		else
		if (m_control > torso_maxvel)
			m_control = torso_maxvel;
		else
		if (fabs(m_control) < torso_deadband)
		{
			m_control = 0;
		}

		Lock ();
		if (m_stopped)
			m_control = 0;
		Unlock ();

		m_arm.VMove (2, m_control);
	}

	void DoRelease(void *) 
	{
		m_arm.Uninitialize ();
	}

public:
	CTorsoController (void) : YARPRateThread ("torso_controller", torso_control_ms, priority_torso)
	{
		m_head_position_channel.Register ("/yarp/arm_thread/head_in");

		m_arm_position.Resize (m_arm.GetNJoints());
		m_arm_position = 0;

		m_head_position.Resize (COGHeadJoints);
		m_head_position = 0;
	}

	virtual ~CTorsoController () {}

	void SetGain (double p, double d)
	{
		Lock ();
		m_pid.setKs (p, d);
		Unlock ();
	}

	void GetGain (double& p, double& d)
	{
		Lock ();
		p = m_pid.getProportional ();
		d = m_pid.getDerivative ();
		Unlock ();
	}

	void SetPositionGain (double p, double d)
	{
		Lock ();
		m_pid_position.setKs (p, d);
		Unlock ();
	}

	void GetPositionGain (double& p, double& d)
	{
		Lock ();
		p = m_pid_position.getProportional ();
		d = m_pid_position.getDerivative ();
		Unlock ();
	}

	void StopTorso (void)
	{
		Lock ();
		m_stopped = true;
		Unlock ();
	}

	void StartTorso (void)
	{
		Lock ();
		m_stopped = false;
		Unlock ();
	}

	void GoTo (double des)
	{
		Lock ();
		m_position_control = true;
		m_desired = des;
		Unlock ();
	}

	void ResetPControl (void)
	{
		Lock ();
		m_position_control = false;
		m_desired = 0;
		Unlock ();
	}
};

//
//
// read arm position and send to a port.
class CEncoderThread : public YARPRateThread
{
	YARPArmControl	m_arm;

	// output last position and speed.
	YARPOutputPortOf <ArmJoints> m_command_channel;		// velocity
	YARPOutputPortOf <ArmJoints> m_position_channel;	// position

	CVisDVector m_position;
	CVisDVector m_speed;
	CVisDVector m_position_o;
	CVisDVector m_speed_o;

	bool m_initialized;

	clock_t m_prev_cycle;

	void DoInit(void *) 
	{
		if (m_arm.Initialize () < 0)
			printf ("encoder thread: can't initialize arm\n");

		m_position = 0;
		m_position_o = 0;
		m_speed = 0;
		m_speed_o = 0;

		Lock ();
		m_initialized = true;
		Unlock ();
	}

	void DoLoop(void *) 
	{
		// computes also an unfiltered version of the speed of the joints.
		// probably very noisy, besides, it should be computed by the YARPArmControl!
		clock_t now = clock();
		m_arm.GetPositions (m_position.data());
		m_speed = (m_position - m_position_o) / ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
		m_prev_cycle = now;

		Lock ();
		m_position_o = m_position;
		m_speed_o = m_speed;
		Unlock ();

		ArmJoints speed;
		memcpy(speed.j, m_speed.data(), sizeof(double) * m_arm.GetNJoints());
		m_command_channel.Content() = speed;
		m_command_channel.Write (); 

		memcpy(speed.j, m_position.data(), sizeof(double) * m_arm.GetNJoints());
		m_position_channel.Content() = speed;
		m_position_channel.Write ();
	}

	void DoRelease(void *) 
	{
		m_arm.Uninitialize ();
	}

public:
	CEncoderThread () : YARPRateThread ("encoder_thread", encoder_ms, priority_encoder) 
	{
		m_initialized = false;
		m_command_channel.Register ("/yarp/arm_thread/v_out");
		m_position_channel.Register ("/yarp/arm_thread/p_out");
		m_prev_cycle = clock();

		m_position.Resize (m_arm.GetNJoints());
		m_position_o.Resize (m_arm.GetNJoints());
		m_speed.Resize (m_arm.GetNJoints());
		m_speed_o.Resize (m_arm.GetNJoints());

		m_position = 0;
		m_position_o = 0;
		m_speed = 0;
		m_speed_o = 0;
	}

	virtual ~CEncoderThread () {}

	inline void GetReading (CVisDVector& p, CVisDVector& v)
	{
		Lock ();
		if (m_initialized)
		{
			p = m_position_o;
			v = m_speed_o;
		}
		else
		{
			p = 0;
			v = 0;
		}
		Unlock ();
	}
};

extern CTorsoController *tthread;

class CCommandReceiver : public YARPThread
{
protected:
	YARPArmControl m_arm;

	YARPInputPortOf<ArmMessage> m_channel;
	CVisDVector m_position;
	CVisDVector m_old_position;
	CVisDVector m_com;

public:
	CCommandReceiver () : m_channel(YARPInputPort::NO_BUFFERS) {}

	void Start (bool wait = true) { Begin(); }
	void Terminate (bool wait = true) { End(); }

	virtual void Body ()
	{
		m_channel.Register ("/yarp/arm_thread/messages");

		if (m_arm.Initialize () < 0)
			return;

		// initialize...
		m_position.Resize (NJOINTS);
		m_position = 0;
		m_old_position.Resize (NJOINTS);
		m_old_position = 0;

		m_com.Resize (NJOINTS);
		m_com = 0;

		for (;;)
		{
			m_channel.Read();
			ArmMessage msg = m_channel.Content();

			switch (msg.type)
			{
			default:
			case ArmMsgNonValid:
				printf ("Arm thread received a non-valid message\n");
				break;


			case ArmMsgMoveTo:
				{
					printf ("ArmMsgMoveTo received\n");

					// position control, it wouldn't work without a complete 
					// gravity compensation module (only 3 is compensated, 7,8 do not need comp).
					for (int j = 3; j <= 8; j++)
					{
						m_arm.SetSpeedSetpoint (j, 150.0);
						m_arm.SetAccelSetpoint (j, 1500.0);
						m_arm.MoveTo (j, msg.j[j]);
					}
				}
				break;

			case ArmMsgMoveToOld:
				{
					printf ("ArmMsgMoveToOld received\n");
					// this is completely open loop, but it looks fine in many situations.
					// precision is unfortunately not guaranteed.
					// 

					int i;
					m_com = 0;
					double MAX = 150.0;
					double max_dist = 0;

					// current arm(s)/torso/hands position.
					m_arm.GetPositions(m_position.data());

					for (i = 3; i <= 8; i++)
					{
						m_com(i+1) = (msg.j[i] - m_position(i+1)); 
						if (fabs(m_com(i+1)) > max_dist)
						{
							max_dist = fabs(m_com(i+1));
						}
					}
			
					MAX = max_dist * 2.0;	
					if (MAX > 150.0)
					{
						MAX = 150.0;
					}
	
					double k = max_dist / MAX;	// .5 seconds or more. (???)
					if (fabs(k) < 0.01)
					{
						m_com = 0;	
					}
					else
					{
						for (i = 3; i <= 8; i++)
						{
							m_com(i+1) /= k;
						}	
					}

					m_arm.VMove (3, 8, m_com.data());
					YARPTime::DelayInSeconds (k);

					m_com = 0;
					m_arm.VMove (3, 8, m_com.data());
				}
				break;

			case ArmMsgTorsoVMove:
				{
					// WARNING: this can't be used together with arm motion.
					// because it sends zero for the arm velocity (this very likely stops the arm).
					printf ("ArmMsgTorsoVMove received\n");
					int i;
					m_com = 0;
					
					for (i = 0; i <= 2; i++)
					{
						m_com(i+1) = msg.j[i];	// this is a velocity!

						// put here some suitable default value.
						m_arm.SetAccelSetpoint (i, torso_a_default);
						m_arm.VMove (m_com.data());
					}
				}
				break;

			case ArmMsgTorsoStop:
				{
					printf ("ArmMsgTorsoStop received\n");
					tthread->StopTorso ();
				}
				break;

			case ArmMsgTorsoStart:
				{
					printf ("ArmMsgTorsoStart received\n");
					tthread->StartTorso ();
				}
				break;

			case ArmMsgTorsoPosition:
				{
					printf ("ArmMsgTorsoPosition received\n");
					tthread->GoTo (msg.j[2]);
				}
				break;

			case ArmMsgTorsoLink2Head:
				{
					printf ("ArmMsgTorsoLink2Head received\n");
					tthread->ResetPControl();
				}
				break;

			case ArmMsgWaveHand:
				{
					printf ("ArmMsgWaveHand received\n");
					m_com = 0;
					int j;
					for (int i = 0; i < 5; i++)
					{
						for (j = 3; j <= 8; j++)
							m_com(j+1) = msg.j[j];

						m_arm.VMove (m_com.data());
						YARPTime::DelayInSeconds (0.25);

						for (j = 3; j <= 8; j++)
							m_com(j+1) = -msg.j[j];

						m_arm.VMove (3, 8, m_com.data());
						YARPTime::DelayInSeconds (0.25);
					}

					m_com = 0;
					m_arm.VMove (m_com.data());
				}
				break;

			case ArmMsgPush:
				{
					printf ("ArmMsgPush received\n");
					m_com = 0;
					for (int j = 3; j <= 8; j++) 
						m_com(j+1) = msg.j[j];

					m_arm.VMove (m_com.data());
					YARPTime::DelayInSeconds (0.5);

					for (j = 3; j <= 8; j++) 
						m_com(j+1) = -msg.j[j];

					m_arm.VMove (3, 8, m_com.data());
					YARPTime::DelayInSeconds (0.5);

					m_com = 0;
					m_arm.VMove (3, 8, m_com.data());
				}
				break;

			case ArmMsgPushOne:
				{
					printf ("ArmMsgPushOne received\n");
					m_com = 0;
					for (int j = 3; j <= 8; j++) 
						m_com(j+1) = msg.j[j];

					m_arm.VMove (m_com.data());
					YARPTime::DelayInSeconds (0.5);

					m_com = 0;
					m_arm.VMove (3, 8, m_com.data());
				}
				break;
			}
		}
	}
};

#endif // __classesarmcontrolh__


