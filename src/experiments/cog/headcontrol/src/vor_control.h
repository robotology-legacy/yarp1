//
// vor_control.h
//

#ifndef __vor_controlh__
#define __vor_controlh__

#include <stdio.h>
#include <conf/lp_layout.h>
#include <conf/tx_data.h>
#include <conf/head_gain.h>

#include <VisMatrix.h>
#include <Models.h>

#include <MotorCtrl.h>

#include "YARPPort.h"

//
// Output must be in encoder ticks
//	gain would probably be much greater than 1
// 
// CSimpleVor. A simple constnt gain VOR.
//
class CSimpleVor
{
protected:
	double m_gain;

public:
	CSimpleVor (double gain)
	{
		m_gain = gain;
	}

	virtual ~CSimpleVor ()
	{
	}

	bool Activable (void)
	{
		return true;
	}

	double Vor (double speed)
	{
		return m_gain * speed;
	}

	void SetGain (double newgain) { m_gain = newgain; }
	double GetGain (void) const { return m_gain; }
};

//
// CRollCtrl. A simple PD controller to align the roll axis with the 
//	gravity vector.
//
class CRollCtrl
{
protected:
	CPdFilter m_roll;

public:
	CRollCtrl (double p, double d)
	{
		m_roll.setKs (p, d);
	}

	virtual ~CRollCtrl ()
	{
	}

	bool Activable (void)
	{
		return true;
	}

	double ClosedLoop (double roll)
	{
		return m_roll.pd (roll);
	}

	void SetGain (double p, double d)
	{
		m_roll.setKs (p, d);
	}

	void GetGain (double *p, double *d)
	{
		*p = m_roll.getProportional ();
		*d = m_roll.getDerivative ();
	}
};

#endif