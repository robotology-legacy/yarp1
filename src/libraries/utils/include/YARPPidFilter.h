// YARPFilter.h
//
// tiny class to implement PID filter
// ( this class is the ancient CPdFilter modified thus to include the integrative)
// gain.
// May 2002 -- by nat
// October 2003 -- by nat
//////////////////////////////////////////////////////////////////////

// $Id: YARPPidFilter.h,v 1.2 2003-10-30 13:32:07 beltran Exp $

#ifndef __YARPPidFilterh__
#define __YARPPidFilterh__

//#ifdef __QNX__
//#include <YARPSafeNew.h>
//#endif

#include <assert.h>
#include <math.h>

class YARPPidFilter  
{
private:
	double oldval;
	double Kp,Kd,Ki;

	// integrative stuff
	double Umax;
	double Sn;

	inline double integrative (double e)
	{
		double temp;
		double u;
		temp = e + Sn;

		u = Ki * temp;

		if (fabs(u) > fabs(Umax))
			u = Ki * Sn;
		else
			Sn = temp;
		
		return u;
	}

public:
	YARPPidFilter(void);
	YARPPidFilter(double kp, double kd=0, double ki = 0, double u = 0.0);
	YARPPidFilter(const YARPPidFilter& f);
	void operator=(const YARPPidFilter& f);
	virtual ~YARPPidFilter(void);

	inline void setKs(double kp, double kd=0.0, double ki=0.0, double u = 0.0)
	{
		Kp = kp;
		Kd = kd;
		Ki = ki;

		Sn = 0;
		Umax = u;
	}

	inline double pid(double error) 
	{
		double u = integrative(error);
		
		double ret=Kp*error+Kd*(error-oldval) + u;
		oldval=error;
		return ret;
	}

	inline double getProportional(void) const { return Kp; }
	inline double getDerivative(void) const { return Kd; }
	inline double getIntegrative(void) const { return Ki; }
};

#endif 