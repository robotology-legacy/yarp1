/*
 *
 * PASA: minimum jerk trajectory generation.
 * 	this code requires some assembly optimization.
 *
 */
 
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
 
#include "Controller.h"

/* global variables */
//bool _actv[JN] = { false, false };
long _x0[JN] = { 0, 0 };
long _xf[JN] = { 0, 0 };
long _distance[JN] = { 0, 0 };

int _t[JN] = { 0, 0 };
__fixed__ _curt[JN] = { 0, 0 };
int _curstep[JN] = { 0, 0 };
int _period = CONTROLLER_PERIOD;	/* in ms */
__fixed__ _step = 0;
bool _ended[JN] = { true, true };

/* local prototypes */
__fixed__ p5 (__fixed__ x);


/* with 2^-6 resolution for the coefficients */
__fixed__ p5 (__fixed__ x)
{
	__longfixed__ accum = 0;
	__fixed__ tmp = x * x;
	tmp *= x;
	accum += (0.15625 * tmp);
	//accum += 0.3125 * tmp;
	tmp *= x;
	accum -= (0.234375 * tmp);
	//accum -= (0.46875 * tmp);
	tmp *= x;
	accum += (0.09375 * tmp);
	//accum += (0.1875 * tmp);
	tmp = (__fixed__)accum;
	return tmp;
}

/* speed must be positive [expressed in ticks/ms] */
int init_trajectory (byte jj, long current, long final, int speed)
{
	unsigned int n;
	
	if (!_ended[jj])
		return -1;
		
	_x0[jj] = current;
	_xf[jj] = final;

	_distance[jj] = _xf[jj] - _x0[jj];
	_t[jj] = __labs (_distance[jj]) / speed;	/* expressed in ms */
	_t[jj] /= _period; 							/* number of steps to complete the trajectory */
	
	n = 0x8000 / _t[jj]; 
	_step = __int2fixed (n); 
		
	_curt[jj] = 0;
	_curstep[jj] = 0;
	
	return 0;
}

/* calculate next step in trajectory generation */
long step_trajectory (byte jj)
{
	long u0 = 0;
	__fixed__ a;
	unsigned int ia;
	
	/* (10 * (t/T)^3 - 15 * (t/T)^4 + 6 * (t/T)^5) * (x0-xf) + x0 */

	if (_curt[jj] == 0)
	{
		_curt[jj] += _step;
		_curstep[jj] ++;
		return _x0[jj];
	}
	else
	if (_curt[jj] < __int2fixed(0x7fff)-_step) ///(_curstep[jj] < _t[jj])
	{
		/* calculate the power factors */
		a = p5 (_curt[jj]);	
		if (a != 0.0)
			ia = 0x8000 / __fixed2int (a);
		else
			ia = 0x7fff; 
			
		/* time */
		_curt[jj] += _step;
		_curstep[jj] ++;

		u0 = _distance[jj] / ia;
		u0 = _L_shl (u0, 6);
		u0 += _x0[jj];
		return u0;
		
		//return _x0[jj] + (_distance[jj] / ia) << 6;
	}			

	_ended[jj] = true;	
	return _xf[jj];
}
