// CPdFilter.cpp: implementation of the CPdFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "PDFilter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPdFilter::CPdFilter(void)
{
	oldval=0;
	Kp=Kd=0;
}

CPdFilter::CPdFilter(double kp,double kd)
{
	oldval=0;
	Kp=kp;
	Kd=kd;
}

CPdFilter::~CPdFilter(void)
{
}

CPdFilter::CPdFilter(const CPdFilter& f)
{
	oldval=f.oldval;
	Kp=f.Kp;
	Kd=f.Kd;
}

void CPdFilter::operator=(const CPdFilter& f)
{
	oldval=f.oldval;
	Kp=f.Kp;
	Kd=f.Kd;
}

