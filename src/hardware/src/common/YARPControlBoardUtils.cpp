#include "YARPControlBoardUtils.h"

// operator overload for the LowLevelPID class. These are used only within 
// the "setGainsSmoothly" functions (see code). SHIFT and OFFSET are not
// considered (they should be both 0 as set by the constructor).
LowLevelPID operator -(const LowLevelPID &l, const LowLevelPID &r)
{
	LowLevelPID tmp;

	tmp.KP = l.KP - r.KP;
	tmp.KD = l.KD - r.KD;
	tmp.KI = l.KI - r.KI;
	tmp.AC_FF = l.AC_FF - r.AC_FF;
	tmp.VEL_FF = l.VEL_FF - r.VEL_FF;
	tmp.FRICT_FF = l.FRICT_FF - r.FRICT_FF;
	tmp.I_LIMIT = l.I_LIMIT - r.I_LIMIT;
	tmp.T_LIMIT = l.T_LIMIT - r.T_LIMIT;

	return tmp;
}

LowLevelPID operator +(const LowLevelPID &l, const LowLevelPID &r)
{
	LowLevelPID tmp;

	tmp.KP = l.KP + r.KP;
	tmp.KD = l.KD + r.KD;
	tmp.KI = l.KI + r.KI;
	tmp.AC_FF = l.AC_FF + r.AC_FF;
	tmp.VEL_FF = l.VEL_FF + r.VEL_FF;
	tmp.FRICT_FF = l.FRICT_FF + r.FRICT_FF;
	tmp.I_LIMIT = l.I_LIMIT + r.I_LIMIT;
	tmp.T_LIMIT = l.T_LIMIT + r.T_LIMIT;

	return tmp;
}

LowLevelPID operator /(const LowLevelPID &l, const double v)
{
	LowLevelPID tmp;

	tmp.KP = l.KP/v;
	tmp.KD = l.KD/v;
	tmp.KI = l.KI/v;
	tmp.AC_FF = l.AC_FF/v;
	tmp.VEL_FF = l.VEL_FF/v;
	tmp.FRICT_FF = l.FRICT_FF/v;
	tmp.I_LIMIT = l.I_LIMIT/v;
	tmp.T_LIMIT = l.T_LIMIT/v;

	return tmp;
}

LowLevelPID operator *(const LowLevelPID &l, const double v)
{
	LowLevelPID tmp;

	tmp.KP = l.KP*v;
	tmp.KD = l.KD*v;
	tmp.KI = l.KI*v;
	tmp.AC_FF = l.AC_FF*v;
	tmp.VEL_FF = l.VEL_FF*v;
	tmp.FRICT_FF = l.FRICT_FF*v;
	tmp.I_LIMIT = l.I_LIMIT*v;
	tmp.T_LIMIT = l.T_LIMIT*v;

	return tmp;
}