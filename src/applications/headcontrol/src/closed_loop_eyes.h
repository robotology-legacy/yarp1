//
// closed_loop_eyes.h
//

#ifndef __closedloopeyesh__
#define __closedloopeyesh__

#include <stdio.h>
#include <conf/lp_layout.h>
#include <conf/tx_data.h>
#include <conf/head_gain.h>

#include <VisMatrix.h>
#include <Models.h>

#include <MotorCtrl.h>
#include <YARPRateThread.h>
#include <YARPTime.h>
#include <CogGaze.h>

#include "channels.h"

//#define XLOGFILE 1

class CPositionControl
{
protected:
	CPdFilter *m_pd;
	int m_njoints;

public:
	CPositionControl (int nj)
	{
		m_njoints = nj;
		m_pd = new CPdFilter[nj];
		assert (m_pd != NULL);

		for (int i = 0; i < nj; i++)
			m_pd[i].setKs (2.0, 0.0);
	}

	virtual ~CPositionControl (void)
	{
		if (m_pd != NULL)
			delete[] m_pd;
	}

	void ClosedLoop (CVisDVector& out, CVisDVector& position, CVisDVector& target)
	{
		for (int i = 0; i < m_njoints; i++)
		{
			out(i+1) = m_pd[i].pd(target(i+1)-position(i+1));
		}
	}
};




// eyes pan and tilt closed loop controller.
// there's no roll control on the basis of visual info.

class CClosedLoopEyes 
{
protected:
	CPdFilter m_eye_tilt;
	CPdFilter m_left;
	CPdFilter m_right;

	CPdFilter m_neckpan;
	CPdFilter m_necktilt;

	CPdFilter m_headtilt;

	StereoPosData	m_color_segm_data;
	int m_period;

	double m_theta, m_phi;
    ArmJoints arm_joints;

	CVisDVector		m_lt;
	CVisDVector		m_rt;

	int m_steps;

	CVisDVector m_cmd;
	int m_njoints;

	// to get the data from.
	CPosVisualChannel *m_channel;

	int m_counter;
	CVisDVector m_old_lt;
	CVisDVector m_old_rt;

#ifdef XLOGFILE
	FILE *fp;
#endif

public:
	CClosedLoopEyes (CPosVisualChannel *ch, int n_joints, int period)
	{
		// shared channel.
		m_channel = ch;

		m_njoints = n_joints;

		m_eye_tilt.setKs (GAIN_eyes_tilt, 0.0);
		m_left.setKs (GAIN_eyes_pan, 0.0);
		m_right.setKs (GAIN_eyes_pan, 0.0);

		m_neckpan.setKs (GAIN_neck_pan, 0.0);
		m_necktilt.setKs (GAIN_neck_tilt, 0.0);

		m_headtilt.setKs (GAIN_head_tilt, 0.0);

		m_lt.Resize (2);
		m_rt.Resize (2);
		m_lt = 0;
		m_rt = 0;

		m_old_lt.Resize (2);
		m_old_rt.Resize (2);
		m_old_lt = 0;
		m_old_rt = 0;

		m_cmd.Resize (m_njoints);
		m_cmd = 0;

		m_steps = 0;
		m_period = period;
		m_counter = 0;

	    for (int i=0; i<3; i++) arm_joints.j[i] = 0;

#ifdef XLOGFILE
		fp = fopen ("position.log", "w");
		assert (fp != NULL);
#endif
	}

	virtual ~CClosedLoopEyes ()
	{
#ifdef XLOGFILE
		if (fp != NULL)
			fclose (fp);
#endif
	}

	void SetOrientation(double n_theta, double n_phi)
	{ 
		m_theta = n_theta;
		m_phi = n_phi;
	}
  
	void SetArm(const ArmJoints& n_arm_joints)
	{
		arm_joints = n_arm_joints;
	}

	int ClosedLoopOrientation (CVisDVector& out, CVisDVector& position, double o7 = 0.0)
	{
		bool data_valid = 1;
		bool converted = 0;

		JointPos jp;
		for (int i=1; i<=7; i++)
		{
			jp(i) = position(i);
		}
		
		CogGaze gaze;
		gaze.Apply(jp,arm_joints);
		float factor = 100.0;
		float factor2 = 200.0;
		m_lt(1) = (m_theta - gaze.theta_middle)*factor;
		if (m_lt(1)>Size/4)  m_lt(1)=Size/4;
		if (m_lt(1)<-Size/4) m_lt(1)=-Size/4;
		if (fabs(m_lt(1))<1)  m_lt(1) = 0;
		m_lt(2) = (m_phi - gaze.phi_middle)*factor2;
		if (m_lt(2)>Size/2)  m_lt(2)=Size/2;
		if (m_lt(2)<-Size/2) m_lt(2)=-Size/2;
		if (fabs(m_lt(2))<1)  m_lt(2) = 0;
		//printf("Dummy target is at %g %g\n", m_lt(1), m_lt(2));
		m_rt(1) = m_lt(1);
		m_rt(2) = m_lt(2);
		data_valid = 1;
		converted = 1;


		// 7 dof head?
		out(1) = m_eye_tilt.pd (-(m_lt(2) + m_rt(2)) / 2.0);

		// build a conjugate movement.
		double tmp = (m_rt(1) + m_lt(1)) / 2.0;
		out(2) = m_right.pd(tmp);
		out(3) = m_left.pd(tmp);

		out(4) = m_headtilt.pd (-position(1) - position(4) - 6000.0);
		out(5) = 0;

		out(6) = m_neckpan.pd(-(position(2)+position(3))/2.0);	// neck pan.
		out(7) = m_necktilt.pd(-position(4) - position(7) + o7); // neck tilt.

		m_cmd = out;

		return 0;
	}

	int ClosedLoopOrientationRight (CVisDVector& out, CVisDVector& position, double o7 = 0.0)
	{
		bool data_valid = 1;
		bool converted = 0;

		JointPos jp;
		for (int i=1; i<=7; i++)
		{
			jp(i) = position(i);
		}
		
		CogGaze gaze;
		gaze.Apply(jp,arm_joints);
		float factor = 100.0;
		float factor2 = 200.0;
		m_lt(1) = (m_theta - gaze.theta_right)*factor;
		if (m_lt(1)>Size/4)  m_lt(1)=Size/4;
		if (m_lt(1)<-Size/4) m_lt(1)=-Size/4;
		if (fabs(m_lt(1))<1)  m_lt(1) = 0;
		m_lt(2) = (m_phi - gaze.phi_right)*factor2;
		if (m_lt(2)>Size/2)  m_lt(2)=Size/2;
		if (m_lt(2)<-Size/2) m_lt(2)=-Size/2;
		if (fabs(m_lt(2))<1)  m_lt(2) = 0;
		//printf("Dummy target is at %g %g\n", m_lt(1), m_lt(2));
		m_rt(1) = m_lt(1);
		m_rt(2) = m_lt(2);
		data_valid = 1;
		converted = 1;


		// 7 dof head?
		out(1) = m_eye_tilt.pd (-(m_lt(2) + m_rt(2)) / 2.0);

		// build a conjugate movement.
		double tmp = (m_rt(1) + m_lt(1)) / 2.0;
		out(2) = m_right.pd(tmp);
		out(3) = m_left.pd(tmp);

		out(4) = m_headtilt.pd (-position(1) - position(4) - 6000.0);
		out(5) = 0;

		out(6) = m_neckpan.pd(-(position(2)+position(3))/2.0);	// neck pan.
		out(7) = m_necktilt.pd(-position(4) - position(7) + o7); // neck tilt.

		m_cmd = out;

		return 0;
	}

	int ClosedLoop (CVisDVector& out, CVisDVector& position, double o7 = 0.0)
	{
		m_channel->GetContent (m_color_segm_data);

		m_lt(1) = m_color_segm_data.xl - Size / 2;
		m_lt(2) = m_color_segm_data.yl - Size / 2;
		m_rt(1) = m_color_segm_data.xr - Size / 2;
		m_rt(2) = m_color_segm_data.yr - Size / 2;

		if (!m_color_segm_data.valid)
		{
			m_steps = 0;
			out = 0;
			m_cmd = 0;
			return -1;
		}

		// 7 dof head?
		out(1) = m_eye_tilt.pd (-(m_lt(2) + m_rt(2)) / 2.0);

		// build a conjugate movement.
		double tmp = (m_rt(1) + m_lt(1)) / 2.0;
		out(2) = m_right.pd(tmp);
		out(3) = m_left.pd(tmp);

		out(4) = m_headtilt.pd (-position(1) - position(4) - 6000.0);
		out(5) = 0;

		out(6) = m_neckpan.pd(-(position(2)+position(3))/2.0);	// neck pan.
		out(7) = m_necktilt.pd(-position(4) - position(7) + o7); // neck tilt.

		m_cmd = out;

#ifdef XLOGFILE
		if (data_valid)
		{
			fprintf (fp, "%f %f %f %f\n", m_lt(1), m_lt(2), m_rt(1), m_rt(2));
			fflush (fp);
		}
#endif

		return 0;
	}

	void SetLeftGain (double p, double d) { m_left.setKs (p, d); }
	void GetLeftGain (double& p, double& d)
	{
		p = m_left.getProportional();
		d = m_left.getDerivative();
	}
	void SetRightGain (double p, double d) { m_right.setKs (p, d); }
	void GetRightGain (double& p, double& d)
	{
		p = m_right.getProportional();
		d = m_right.getDerivative();
	}
	void SetTiltGain (double p, double d) { m_eye_tilt.setKs (p, d); }
	void GetTiltGain (double& p, double& d)
	{
		p = m_eye_tilt.getProportional();
		d = m_eye_tilt.getDerivative();
	}

	void SetNeckTiltGain (double p, double d) 
	{
		m_necktilt.setKs (p, d);
	}
};

#endif
