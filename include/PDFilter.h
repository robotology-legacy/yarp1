// CPdFilter.h: interface for the CPdFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PDFILTER_H__BAB5031B_EE1C_11D1_A91E_00A02493CB00__INCLUDED_)
#define AFX_PDFILTER_H__BAB5031B_EE1C_11D1_A91E_00A02493CB00__INCLUDED_

#ifdef WIN32

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#endif

#ifdef __QNX__
#include <YARPSafeNew.h>
#endif

#include <assert.h>

class CPdFilter  
{
private:
	double oldval;
	double Kp,Kd;

public:
	CPdFilter(void);
	CPdFilter(double kp,double kd);
	CPdFilter(const CPdFilter& f);
	void operator=(const CPdFilter& f);
	virtual ~CPdFilter(void);

	inline void setKs(double kp,double kd)
	{
		Kp=kp;
		Kd=kd;
	}

	inline double pd(double error) 
	{
		double ret=Kp*error+Kd*(error-oldval);
		oldval=error;
		return ret;
	}

	inline double getProportional(void) const { return Kp; }
	inline double getDerivative(void) const { return Kd; }
};

#endif // !defined(AFX_PDFILTER_H__BAB5031B_EE1C_11D1_A91E_00A02493CB00__INCLUDED_)
