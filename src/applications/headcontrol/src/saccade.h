//
// saccade.h
//

#ifndef __saccadeh__
#define __saccadeh__

#include <stdio.h>
#include <conf/lp_layout.h>
#include <conf/tx_data.h>
#include <conf/head_gain.h>

#include <VisMatrix.h>
#include <Models.h>

#include <MotorCtrl.h>

#include "YARPPort.h"

//
// a simple saccadic behavior.
//	no map, just a large gain... roughly tuned to generate a saccade.
//
class CLinearSaccade
{
protected:
	double m_gain_eyes;
	double m_gain_tilt;

	CPosVisualChannel *m_channel;
	int m_joints;
	int m_period;

	int m_refractory;
	int m_refractory_counter;
	int m_counter;

	StereoPosData	m_data;
	StereoPosData   m_dummy;
	CVisDVector m_out;

public:
	CLinearSaccade (CPosVisualChannel *ch, int n_joints, int period)
	{
		m_gain_eyes = GAIN_saccade_pan;
		m_gain_tilt = GAIN_saccade_tilt;

		m_channel = ch;
		m_joints = n_joints;
		m_period = period;

		memset (&m_data, 0, sizeof (StereoPosData));

		m_refractory = 20 * 20/period;
		m_refractory_counter = 0;
		m_counter = 0;

		m_out.Resize (n_joints);
		m_out = 0;
	}

	virtual ~CLinearSaccade ()
	{
	}

	void SetGain (double pan, double tilt)
	{
		m_gain_tilt = tilt;
		m_gain_eyes = pan;
	}

	void GetGain (double *pan, double *tilt)
	{
		*pan = m_gain_eyes;
		*tilt = m_gain_tilt;
	}

	// test whether a saccade can be generated.
	bool Activable (void)
	{
		m_channel->GetContent (m_data);
//		if (m_channel->Read (&m_data))
//		{
			if (m_data.valid != 0)
			{
				if (!IsInFovea (m_data) && m_refractory_counter <= 0)
				{
					m_refractory_counter = m_refractory;
					m_counter = 0;
					return true;
				}
			}
//		}

		m_refractory_counter --;
		return false;
	}

	int Saccade (CVisDVector& out)
	{
		if (m_counter == 0)
		{
			//bool data_valid = m_channel->Read (&m_data);

			//if (!data_valid)
			//{
			//	out = 0;
			//	m_out = 0;
			//	return -1;
			//}

			m_channel->GetContent (m_data);
			//cout << "valid saccade" << endl;

			const double m_lx = m_data.xl - Size / 2;
			const double m_ly = m_data.yl - Size / 2;
			const double m_rx = m_data.xr - Size / 2;
			const double m_ry = m_data.yr - Size / 2;

			out(1) = m_gain_tilt * (-(m_ly + m_ry) / 2.0);
			const double tmp = (m_rx + m_lx) / 2.0;
			out(2) = m_gain_eyes * tmp;
			out(3) = m_gain_eyes * tmp;
			out(4) = 0;
			out(5) = 0;
			out(6) = 0;
			out(7) = 0;

			m_out = out;
			m_counter ++;
		}
		else
		{
			//m_channel->Read (&m_dummy);

			out = m_out;
			m_counter ++;

			//cout << "saccade step" << endl;
		}

		if (m_counter >= 3)
		{
			return -1;
			// end of saccade.
		}
		else
		{
			return 0;
			// ok to saccade.
		}
	}

	bool IsInFovea (StereoPosData& data)
	{
		const double m_lx = data.xl - Size / 2;
		const double m_ly = data.yl - Size / 2;
		const double m_rx = data.xr - Size / 2;
		const double m_ry = data.yr - Size / 2;

		const int thr = 22 * 22;
		if ((m_lx * m_lx + m_ly * m_ly < thr) &&
			(m_rx * m_rx + m_ry * m_ry < thr))
			return true;
		
		return false;
	}
};

#endif