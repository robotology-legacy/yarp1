//
// classes.h
//
#ifndef __classesh__
#define __classesh__

#include "headcontrol.h"

extern char INI_FILENAME[];
extern char MEI_DEVICENAME[];

typedef enum
{
	AS_Fixating,
	AS_ClosedLoop,
	AS_Saccade,
	AS_Null
} 
ControlThreadStatus;

enum
{
    JOINT_TORSO_ROLL = 0,
    JOINT_TORSO_PITCH = 1,
    JOINT_TORSO_YAW = 2,
};

const int period_ms = 20;
const int encoder_ms = 5;

class CArmPort : public YARPInputPortOf<ArmJoints>
{
public:
  YARPSemaphore state_mutex;
  ArmJoints state_arm;
  
  CArmPort() : state_mutex(1)
    {
      printf("*** torso ticker begins\n");
      for (int i=0; i<3; i++)
	{
	  state_arm.j[i] = 0;
	}
    }
  
  virtual void OnRead()
    {
      assert(Read(0));
      state_mutex.Wait();
      state_arm = Content();
      //printf("*** torso input roll:%g pitch:%g yaw:%g\n",
      //state_arm.j[JOINT_TORSO_ROLL],
      //state_arm.j[JOINT_TORSO_PITCH],
      //state_arm.j[JOINT_TORSO_YAW]);
      state_mutex.Post();
    }
  
  void Lock() { state_mutex.Wait(); }
  void Unlock() { state_mutex.Post(); }
  ArmJoints& GetState() { return state_arm; }
};


class CSerialThread : public YARPThread
{
protected:
	YARPSemaphore mutex;
	YARPInertialSensor m_isd;
	CVisDVector m_inertial, m_out;
	YARPOutputPortOf <GyroPos> m_gyrochannel;

public:
	CSerialThread () : mutex(1)
	{
		m_gyrochannel.Register (GYROCHANNELNAME);
		YARPTime::DelayInSeconds (0.5);
	}

	virtual ~CSerialThread () {}

	inline void Start (bool w = false) { Begin(); }
	inline void Terminate (bool w = false) { End(); }

	inline void Lock()
	{	
		mutex.Wait ();
	}

	inline void Unlock()
	{
		mutex.Post ();
	}

	virtual void Body (void)
	{
		// init.
		m_isd.Initialize ();
		m_inertial.Resize (6);
		m_inertial = 0;
		m_out.Resize (6);
		m_out = 0;
		
		while (1)
		{
			m_isd.GetAll (m_inertial.data());

			Lock ();
			m_out = m_inertial;
			Unlock ();

			// this goes out as fast as possible.
			m_gyrochannel.Content().r = m_inertial(3);
			m_gyrochannel.Content().p = m_inertial(2);
			m_gyrochannel.Content().y = m_inertial(1);
			m_gyrochannel.Write();
		}

		//m_isd.Uninitialize ();
	}

	void GetReading (CVisDVector& o)
	{
		Lock ();
		o = m_out;
		Unlock ();
	}
};

class CEncoderThread : public YARPRateThread
{
protected:
	YARPHeadControl	m_head;

	// output last position and speed.
	YARPOutputPortOf <JointSpeed> m_command_channel;	// velocity
	YARPOutputPortOf <JointPos>   m_position_channel;	// position

	CVisDVector m_position;
	CVisDVector m_speed;
	CVisDVector m_position_o;
	CVisDVector m_speed_o;

	bool m_initialized;

	void DoInit(void *) 
	{
		m_head.Initialize (MEI_DEVICENAME, INI_FILENAME);

		m_position.Resize (m_head.GetNJoints());
		m_position_o.Resize (m_head.GetNJoints());
		m_speed.Resize (m_head.GetNJoints());
		m_speed_o.Resize (m_head.GetNJoints());

		m_position = 0;
		m_position_o = 0;
		m_speed = 0;
		m_speed_o = 0;

		m_command_channel.Register (COMMANDCHANNELNAME);
		m_position_channel.Register (POSITIONCHANNELNAME);

		Lock ();
		m_initialized = true;
		Unlock ();
	}
	
	void DoLoop(void *) 
	{
		m_head.Input ();
		m_head.GetActualPosition (m_position.data());
		m_head.GetActualVelocity (m_speed.data());

		Lock ();
		m_position_o = m_position;
		m_speed_o = m_speed;
		Unlock ();

		JointSpeed speed;
		speed.j1 = m_speed(1);
		speed.j2 = m_speed(2);
		speed.j3 = m_speed(3);
		speed.j4 = m_speed(4);
		speed.j5 = m_speed(5);
		speed.j6 = m_speed(6);
		speed.j7 = m_speed(7);

		m_command_channel.Content() = speed;
		m_command_channel.Write ();

		JointPos pos;
		pos.j1 = m_position(1);
		pos.j2 = m_position(2);
		pos.j3 = m_position(3);
		pos.j4 = m_position(4);
		pos.j5 = m_position(5);
		pos.j6 = m_position(6);
		pos.j7 = m_position(7);
		m_position_channel.Content() = pos;
		m_position_channel.Write ();
	}

	void DoRelease(void *) 
	{
		m_head.Uninitialize ();
	}

public:
	CEncoderThread () : YARPRateThread ("encoder_thread", encoder_ms) 
	{
		m_initialized = false;
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

//#define XXLOGFILE 1

class CControlThread : public YARPRateThread
{
	YARPHeadControl	m_head;

	CSerialThread	m_isdthread;
	CEncoderThread	m_ethread;
	CArmPort        m_armport;

	CVisDVector		m_final_control;
	CVisDVector		m_position;
	CVisDVector		m_speed;
	CVisDVector     m_inertial;

	ControlThreadStatus m_status;	// thread status.

	// channels.
	CPosVisualChannel	m_channel_pos;
	CDisparityChannel   m_disparity_ch;
	CExternalMessages	m_external_msg;

	// behaviors.
	CClosedLoopEyes		m_eyes;
	CSimpleVor			m_vor_yaw;
	CSimpleVor			m_vor_pitch;
	CRollCtrl			m_roll;
	CDisparityCtrl		m_d_ctrl;
	CLinearSaccade		m_saccade_ctrl;
	CPositionControl	m_position_ctrl;

	// additional data.
	CVisDVector m_closed_loop_eyes;
	CVisDVector m_vor;
	CVisDVector m_vergence;
	CVisDVector m_saccade;

	clock_t m_prev_cycle;
	bool m_timing;
	bool m_needtostop;

	double m_lastmessagetime;

#ifdef XXLOGFILE
	FILE *fp;
#endif

protected:
	ControlThreadStatus ControlLogic (void)
	{
		if (m_saccade_ctrl.Activable ())
		{
			return AS_Saccade;
		}

		return AS_Fixating;
	}

	void DoInit(void *)
	{
#ifdef XXLOGFILE
		fp = fopen ("logfile.log", "w");
		assert (fp != NULL);
#endif

		m_head.Initialize (MEI_DEVICENAME, INI_FILENAME);

		m_isdthread.Start ();
		m_ethread.Start (0);
	
		m_armport.Register (ARMCHANNELNAME);
		m_external_msg.Register (MESSAGESCHANNELNAME);
		m_channel_pos.Register (ATTPOSITIONCHANNELNAME);
		m_disparity_ch.Register (ATTDISPARITYCHANNELNAME);

		m_closed_loop_eyes.Resize (m_head.GetNJoints());
		m_vor.Resize (m_head.GetNJoints());
		m_vergence.Resize (m_head.GetNJoints());
		m_saccade.Resize (m_head.GetNJoints());

		m_closed_loop_eyes = 0;
		m_vor = 0;
		m_vergence = 0;
		m_saccade = 0;

		m_final_control.Resize (m_head.GetNJoints());
		m_position.Resize (m_head.GetNJoints());
		m_speed.Resize (m_head.GetNJoints ());

		// 3 positional components and 3 velocities.
		m_inertial.Resize (6);
		m_inertial = 0;

		m_closed_loop_eyes = 0;
		m_final_control = 0;

		m_status = AS_Null;

		m_head.EnableAxis(m_head.EyeTilt());
		m_head.EnableAxis(m_head.RightEye());
		m_head.EnableAxis(m_head.LeftEye());
		m_head.EnableAxis(m_head.NeckPan());
		m_head.EnableAxis(m_head.NeckTilt());
		m_head.EnableAxis(m_head.HeadTilt());
		m_head.EnableAxis(m_head.HeadRoll());

		m_prev_cycle = clock();
		m_timing = false;

		m_lastmessagetime = -1000;

		m_needtostop = true;
	}

	void DoLoop(void *)
	{
		clock_t now = clock();
		if (m_timing)
			cout << ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC) << endl;

		Lock ();
		m_ethread.GetReading (m_position, m_speed);
		m_isdthread.GetReading (m_inertial);
	
		// 
		switch (m_status)
		{

		//
		//
		case AS_Fixating:
			{
				m_status = ControlLogic ();

				// process additional messages.
				// it doesn't saccade while processing a message.
				HeadMessage msg;
				m_external_msg.GetMessage (msg);
				m_lastmessagetime = m_external_msg.GetLastMessageTime();

				switch (msg.type)
				{
				default:
				case HeadMsgNonValid:
					{
						// DEFAULT PROCESSING
						// compute here stabilization command.
						// gaze stabilization.
						m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
						m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
						m_vor(5) = m_roll.ClosedLoop (m_inertial(3));

						m_d_ctrl.ClosedLoop (m_vergence, m_position);
						m_eyes.ClosedLoop (m_closed_loop_eyes, m_position);
					}
					break;

				case HeadMsgOffset:
					{
						// offset movement in some of the joints.
						m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
						m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
						m_vor(5) = m_roll.ClosedLoop (m_inertial(3) + msg.j5);
						m_d_ctrl.ClosedLoop (m_vergence, m_position);
						m_eyes.ClosedLoop (m_closed_loop_eyes, m_position, msg.j7);

						m_status = AS_Fixating;
					}
					break;

				case HeadMsgStopNeckMovement:
					{
						m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
						m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
						m_vor(5) = m_roll.ClosedLoop (m_inertial(3));

						m_d_ctrl.ClosedLoop (m_vergence, m_position);
						m_eyes.ClosedLoop (m_closed_loop_eyes, m_position);

						for (int i = 4; i <= 7; i++)
						{
							m_closed_loop_eyes(i) = 0.0;
						}

						m_status = AS_Fixating;
					}
					break;

				case HeadMsgStopAll:
					{
						m_closed_loop_eyes = 0.0;
						m_vergence = 0.0;
						m_vor = 0.0;

						m_status = AS_Fixating;
					}
					break;

				case HeadMsgStopAllVisual:
					{
						m_closed_loop_eyes = 0.0;
						m_vergence = 0.0;

						m_status = AS_Fixating;
					}
					break;

				case HeadMsgMoveToPosition:
					{
						// use m_closed_loop_eyes as target.
						m_closed_loop_eyes(1) = msg.j1;
						m_closed_loop_eyes(2) = msg.j2;
						m_closed_loop_eyes(3) = msg.j3;
						m_closed_loop_eyes(4) = msg.j4;
						m_closed_loop_eyes(5) = msg.j5;
						m_closed_loop_eyes(6) = msg.j6;
						m_closed_loop_eyes(7) = msg.j7;

						m_position_ctrl.ClosedLoop (m_closed_loop_eyes, m_position, m_closed_loop_eyes);
						
						m_vergence = 0.0;
						m_vor = 0.0;

						// do not saccade while executing ext message commands.
						m_status = AS_Fixating;
					}
					break;
				  
				case HeadMsgMoveToOrientation:
					{
						double request_theta, request_phi;
						request_theta = msg.j1;
						request_phi = msg.j2;

						// msg.j3 used for setting the timeout.
						double timeout = msg.j3;
						if (timeout < 1e-10)
						{
							timeout = 1e100;
						}

						// msg.j4/5/6 reserved for the left eye.
						// msg.j7 desired vergence in radians.

						m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
						m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
						m_vor(5) = m_roll.ClosedLoop (m_inertial(3));

						if (msg.j7 > 0)
						{
							msg.j7 *= (43500 * 2 / pi);

							// control vergence.
							m_vergence = 0;
							m_vergence(2) = -m_d_ctrl.Vergence (msg.j7 - (m_position(2)-m_position(3)));
							m_vergence(3) = -m_vergence(2);
						}
						else
							m_vergence = 0;

						m_eyes.SetOrientation(request_theta, request_phi);
						m_armport.Lock();
						m_eyes.SetArm(m_armport.GetState());
						m_armport.Unlock();
						m_eyes.ClosedLoopOrientation (m_closed_loop_eyes, m_position);

						m_status = AS_Fixating;
						
						if (YARPTime::GetTimeAsSeconds() - m_lastmessagetime > timeout)
						{
							memset (&msg, 0, sizeof(HeadMessage));
							m_external_msg.SetMessage (msg);						
						}
					}
					break;

				case HeadMsgMoveToOrientationRight:
					{
						double request_theta, request_phi;
						request_theta = msg.j1;
						request_phi = msg.j2;

						// msg.j3 used for setting the timeout.
						double timeout = msg.j3;
						if (timeout < 1e-10)
						{
							timeout = 1e100;
						}

						// msg.j4/5/6 reserved for the left eye.
						// msg.j7 desired vergence in radians.

						m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
						m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
						m_vor(5) = m_roll.ClosedLoop (m_inertial(3));

						if (msg.j7 > 0)
						{
							msg.j7 *= (43500 * 2 / pi);

							// control vergence.
							m_vergence = 0;
							m_vergence(2) = -m_d_ctrl.Vergence (msg.j7 - (m_position(2)-m_position(3)));
							m_vergence(3) = -m_vergence(2);
						}
						else
							m_vergence = 0;

						m_eyes.SetOrientation(request_theta, request_phi);
						m_armport.Lock();
						m_eyes.SetArm(m_armport.GetState());
						m_armport.Unlock();
						m_eyes.ClosedLoopOrientationRight (m_closed_loop_eyes, m_position);

						m_status = AS_Fixating;
						
						if (YARPTime::GetTimeAsSeconds() - m_lastmessagetime > timeout)
						{
							memset (&msg, 0, sizeof(HeadMessage));
							m_external_msg.SetMessage (msg);						
						}
					}
					break;

				} // end switch (internal).

				// FINAL CONTROL VALUE (sum the components).
				m_final_control = m_vor + m_vergence + m_closed_loop_eyes;

			}
			break;

		//
		//
		case AS_Saccade:
			{
				// DEFAULT PROCESSING
				// compute here stabilization command.
				// gaze stabilization.
				m_vor(2) = m_vor(3) = m_vor_yaw.Vor (m_inertial(4));
				m_vor(1) = m_vor_pitch.Vor (m_inertial(5));
				m_vor(5) = m_roll.ClosedLoop (m_inertial(3));

				m_d_ctrl.ClosedLoop (m_vergence, m_position);
				m_eyes.ClosedLoop (m_closed_loop_eyes, m_position);

				if (m_saccade_ctrl.Saccade (m_saccade) < 0)
				{
					m_status = AS_Fixating;
				}

				m_final_control = m_saccade;
				m_final_control += (m_vor + m_vergence + m_closed_loop_eyes);
			}
			break;


		//
		//
		default:
		case AS_Null:
			{
				if (m_needtostop)
				{
					m_final_control = 0;
					m_needtostop = false;
				}
			}
			break;
		}

		if (m_status != AS_Null)
		{
			int speedcheck = 0;
			speedcheck |= ((fabs(m_final_control(1)) > 20000) ? 0x01 : 0x00);
			speedcheck |= ((fabs(m_final_control(2)) > 90000) ? 0x02 : 0x00);
			speedcheck |= ((fabs(m_final_control(3)) > 90000) ? 0x04 : 0x00);
			speedcheck |= ((fabs(m_final_control(4)) > 7000) ? 0x08 : 0x00);
			speedcheck |= ((fabs(m_final_control(5)) > 7000) ? 0x10 : 0x00);
			speedcheck |= ((fabs(m_final_control(6)) > 15000) ? 0x20 : 0x00);
			speedcheck |= ((fabs(m_final_control(7)) > 7000) ? 0x40 : 0x00);
			
			// check...
			if (speedcheck != 0)
			{
				//printf ("speed outside limits %x\n", speedcheck);
				if (speedcheck & 0x01)
					m_final_control(1) = Saturate (m_final_control(1), 20000.0);
				if (speedcheck & 0x02)
					m_final_control(2) = Saturate (m_final_control(2), 90000.0);
				if (speedcheck & 0x04)
					m_final_control(3) = Saturate (m_final_control(3), 90000.0);
				if (speedcheck & 0x08)
					m_final_control(4) = Saturate (m_final_control(4), 7000.0);
				if (speedcheck & 0x10)
					m_final_control(5) = Saturate (m_final_control(5), 7000.0);
				if (speedcheck & 0x20)
					m_final_control(6) = Saturate (m_final_control(6), 15000.0);
				if (speedcheck & 0x40)
					m_final_control(7) = Saturate (m_final_control(7), 7000.0);
			}

			// send control to head.
			m_head.VelocityCommand(m_final_control.data(), period_ms * 1e-3);

#ifdef XXLOGFILE
		fprintf (fp, "%d %f %f %f %f %f %f %f\n", 
			 ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC),
			 m_final_control(1),
			 m_final_control(2),
			 m_final_control(3),
			 m_final_control(4),
			 m_final_control(5),
			 m_final_control(6),
			 m_final_control(7));

		fprintf (fp, "%f %f %f %f %f %f %f\n",
			m_vor(1),
			m_vor(2),
			m_vor(3),
			m_vor(4),
			m_vor(5),
			m_vor(6),
			m_vor(7));

		fprintf (fp, "%f %f %f %f %f %f %f\n",
			m_vergence(1),
			m_vergence(2),
			m_vergence(3),
			m_vergence(4),
			m_vergence(5),
			m_vergence(6),
			m_vergence(7));

		fprintf (fp, "%f %f %f %f %f %f %f\n",
			m_closed_loop_eyes(1),
			m_closed_loop_eyes(2),
			m_closed_loop_eyes(3),
			m_closed_loop_eyes(4),
			m_closed_loop_eyes(5),
			m_closed_loop_eyes(6),
			m_closed_loop_eyes(7));

		fprintf (fp, "\n");
		fflush (fp);
#endif
		}
		else
		{
			m_final_control = 0;
			//m_head.VelocityCommand(m_final_control.data(), period_ms * 1e-3);
		}

		Unlock ();
		m_prev_cycle = now;
	}

	inline double Saturate (double value, double limit)
	{
		if (value > limit)
			return limit;
		else
		if (value < -limit)
			return -limit;

		return value;
	}

	void DoRelease(void *)
	{
		printf ("DoRelease called\n");
		fflush (stdout);

		m_final_control = 0;
		m_head.VelocityCommand(m_final_control.data(), period_ms * 1e-3);

		printf ("Head stopped v=0\n");
		m_head.Uninitialize ();

		m_isdthread.Terminate ();
		m_ethread.Terminate (0);

#ifdef XXLOGFILE
		if (fp != NULL)
			fclose (fp);
#endif
	}

public:
	CControlThread () : YARPRateThread ("control_thread", period_ms), 
						m_channel_pos (period_ms),
						m_eyes(&m_channel_pos, 7, period_ms),
						m_saccade_ctrl(&m_channel_pos, 7, period_ms),
						m_vor_yaw (GAIN_vor_yaw),
						m_vor_pitch (GAIN_vor_pitch),
						m_roll (GAIN_stabilize_roll, 0.0),
						m_disparity_ch (period_ms),
						m_d_ctrl (&m_disparity_ch, 7, period_ms),
						m_position_ctrl (7)
	{
	}

	bool IsNull (void) { return (m_status == AS_Null) ? true : false; }
	void GoToNull (void) 
	{
		Lock ();
		m_status = AS_Null;
		m_needtostop = true;
		m_final_control = 0;
		m_head.VelocityCommand(m_final_control.data(), period_ms * 1e-3);
		Unlock ();
	}

	void RestartControl (void)
	{
		Lock ();
		m_status = AS_Fixating;
		Unlock ();
	}

	void SetFeedbackEyesGain (double l, double r, double t)
	{
		Lock ();
		m_eyes.SetLeftGain (l, 0.0);
		m_eyes.SetRightGain (r, 0.0);
		m_eyes.SetTiltGain (t, 0.0);
		Unlock ();
	}

	void GetFeedbackEyesGain (double *l, double *r, double *t)
	{
		double tmp;
		Lock ();
		m_eyes.GetLeftGain (*l, tmp);
		m_eyes.GetRightGain (*r, tmp);
		m_eyes.GetTiltGain (*t, tmp);
		Unlock ();
	}

	void SetRollGain (double g)
	{
		Lock ();
		m_roll.SetGain (g, 0.0);
		Unlock ();
	}

	void GetRollGain (double *g)
	{
		double tmp;
		Lock ();
		m_roll.GetGain (g, &tmp);
		Unlock ();
	}

	void SetVorGain (double gy, double gt)
	{
		Lock ();
		m_vor_yaw.SetGain (gy);
		m_vor_pitch.SetGain (gt);
		Unlock ();
	}

	void GetVorGain (double *gy, double *gt)
	{
		Lock ();
		*gy = m_vor_yaw.GetGain ();
		*gt = m_vor_pitch.GetGain ();
		Unlock ();
	}

	void SetDisparityGain (double g)
	{
		Lock ();
		m_d_ctrl.SetGain (g, 0.0);
		Unlock ();
	}

	void GetDisparityGain (double *g)
	{
		double tmp;
		Lock ();
		m_d_ctrl.GetGain (g, &tmp);
		Unlock ();
	}

	void SetSaccadeGain (double p, double t)
	{
		Lock ();
		m_saccade_ctrl.SetGain (p, t);
		Unlock ();
	}

	void GetSaccadeGain (double *p, double *t)
	{
		Lock ();
		m_saccade_ctrl.GetGain (p, t);
		Unlock ();
	}

	void ToggleTiming (void)
	{
		m_timing = !m_timing;
	}

	void GetLastControlValue (double *ctrl)
	{
		Lock ();
		memcpy (ctrl, m_final_control.data(), sizeof(double) * m_head.GetNJoints());
		Unlock ();
	}
};

#endif
