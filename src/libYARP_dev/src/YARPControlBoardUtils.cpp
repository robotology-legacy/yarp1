/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
/// $Id: YARPControlBoardUtils.cpp,v 1.3 2004-08-02 13:30:55 eshuy Exp $
///
///

#include <yarp/YARPControlBoardUtils.h>

// operator overload for the LowLevelPID class. These are used only within 
// the "setGainsSmoothly" functions (see code). SHIFT is not
// considered (it should be both 0 as set by the constructor).
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
	tmp.OFFSET = l.OFFSET - r.OFFSET;

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
	tmp.OFFSET = l.OFFSET + r.OFFSET;

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
	tmp.OFFSET = l.OFFSET/v;

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
	tmp.OFFSET = l.OFFSET*v;

	return tmp;
}
