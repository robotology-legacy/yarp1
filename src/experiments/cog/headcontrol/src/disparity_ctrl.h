//
// disparity_ctrl.h
//

#ifndef __disparity_ctrlh__
#define __disparity_ctrlh__

#include <stdio.h>
#include <conf/lp_layout.h>
#include <conf/tx_data.h>
#include <conf/head_gain.h>

#include <VisMatrix.h>
#include <Models.h>

#include <MotorCtrl.h>

#include "channels.h"

//#define LOGFILE 1

class CDisparityCtrl 
{
protected:
	CPdFilter m_vergence;

	DisparityData m_data;

	CDisparityChannel *m_channel;
	int m_njoints;
	int m_period;

	CVisDVector m_cmd;

#ifdef LOGFILE
	FILE *fp;
#endif

public:
	CDisparityCtrl (CDisparityChannel *ch, int n_joints, int period)
	{
		m_channel = ch;
		m_njoints = n_joints;
		m_period = period;

		m_vergence.setKs (GAIN_vergence, 0.0);

		m_cmd.Resize (n_joints);
		m_cmd = 0;

#ifdef LOGFILE
		fp = fopen ("disparity.log", "w");
		assert (fp != NULL);
#endif
	}

	virtual ~CDisparityCtrl ()
	{
#ifdef LOGFILE
		if (fp != NULL)
			fclose (fp);
#endif
	}

	bool Activable (void)
	{
		return true;
	}

	int ClosedLoop (CVisDVector& out, CVisDVector& position)
	{
		m_channel->GetContent (m_data);

		double tmp = m_vergence.pd (-m_data.disparity);
		out(2) = -tmp;
		out(3) = tmp;
		m_cmd = out;

#ifdef LOGFILE
		fprintf (fp, "%f\n", m_data.disparity);
		fflush (fp);
#endif
		return 0;
	}

	double Vergence (double error)
	{
		return m_vergence.pd (-error * 0.0024);
	}

	void SetGain (double p, double d)
	{
		m_vergence.setKs (p, d);
	}

	void GetGain (double *p, double *d)
	{
		*p = m_vergence.getProportional ();
		*d = m_vergence.getDerivative ();
	}
};


#endif