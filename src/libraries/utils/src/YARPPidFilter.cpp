#include "YARPPidFilter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
YARPPidFilter::YARPPidFilter(void)
{
	oldval=0;
	Kp=0;
	Kd=0;
	Ki=0;

	Umax = 0.0;
	Sn = 0.0;
}

YARPPidFilter::YARPPidFilter(double kp,double kd,double ki,double u)
{
	oldval=0;
	Kp=kp;
	Kd=kd;
	Ki=ki;

	Umax = u;
	Sn = 0.0;
}

YARPPidFilter::~YARPPidFilter(void)
{
}

YARPPidFilter::YARPPidFilter(const YARPPidFilter& f)
{
	oldval=f.oldval;
	Kp=f.Kp;
	Kd=f.Kd;
	Ki=f.Ki;

	Umax = f.Umax;
	Sn = f.Sn;
}

void YARPPidFilter::operator=(const YARPPidFilter& f)
{
	oldval=f.oldval;
	Kp=f.Kp;
	Kd=f.Kd;
	Ki=f.Ki;
	
	Umax = f.Umax;
	Sn = f.Sn;
}