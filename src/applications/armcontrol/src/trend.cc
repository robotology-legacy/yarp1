/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "armcontrol.h"
#include "abimport.h"
#include "proto.h"

double scalefactor[20];
short trenddata[20];
bool dtorque = false;

const int TRENDMIN = 3;
const int TRENDMAX = 8;

int
activate_timer( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// read new data, prepare array and send to trend widget.
	// ABW_base_trend
	if (!dtorque)
	{
		arm->GetPositions (actual_pos);

		for (int i = TRENDMIN; i <= TRENDMAX; i++)
		{
			actual_pos[i] -= zero_pos[i];

			if (scalefactor[i] != 0.0)
			{
				actual_pos[i] /= scalefactor[i];
			}
			else
			{
				actual_pos[i] = 0;
			}

			if (actual_pos[i] > 32767)
				trenddata[i-TRENDMIN] = 32767;
			else
			if (actual_pos[i] < -32768)
				trenddata[i-TRENDMIN] = -32768;
			else
				trenddata[i-TRENDMIN] = short(actual_pos[i]);
		}
	}
	else
	{
		// reuse actual_pos.
		arm->GetErrors (actual_pos);

		for (int i = TRENDMIN; i <= TRENDMAX; i++)
		{
			if (scalefactor[i] != 0.0)
			{
				actual_pos[i] /= scalefactor[i];
			}
			else
			{
				actual_pos[i] = 0;
			}

			if (actual_pos[i] > 32767)
				trenddata[i-TRENDMIN] = 32767;
			else
			if (actual_pos[i] < -32768)
				trenddata[i-TRENDMIN] = -32768;
			else
				trenddata[i-TRENDMIN] = short(actual_pos[i]);
		}
	}

	PtArg_t arg;
	PtSetArg (&arg, Rt_ARG_TREND_DATA, trenddata, LEFTMAXAXIS-LEFTMINAXIS+1);
	PtSetResources (ABW_base_trend, 1, &arg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
realize_trend( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// assumes there are 12 joints.
	int trend_color_array[12] = {    Pg_WHITE,
									 Pg_MAGENTA,
									 Pg_GREEN,
									 Pg_RED,
									 Pg_YELLOW,
									 Pg_BLUE,
									 Pg_WHITE,
									 Pg_MAGENTA,
									 Pg_GREEN,
									 Pg_RED,
									 Pg_YELLOW,
									 Pg_BLUE
							   };

	RtTrendAttr_t one_attr, several_attr[6];
	PtArg_t args[2];

	/* Set up the color list. */
	PtSetArg (&args[0], Rt_ARG_TREND_COLOR_LIST, trend_color_array, TRENDMAX-TRENDMIN+1);
	PtSetResources (widget, 1, args);

	/* Map the trends to colors. */
	for (int i = 0; i < TRENDMAX-TRENDMIN+1; i++)
		several_attr[i].map = i+1;

	PtSetArg (&args[0], Rt_ARG_TREND_ATTRIBUTES, several_attr, 0);
	PtSetResources (widget, 1, args);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_trend( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// toggle timer on/off and read and set limits of trend widget.
	PtArg_t arg[2];
	PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
	PtGetResources (widget, 1, &arg[0]);
	bool active = ((arg[0].value & Pt_SET) != 0) ? true : false;

	PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
	PtGetResources (ABW_base_dtorque_toggle, 1, &arg[0]);
	
	dtorque = ((arg[0].value & Pt_SET) != 0) ? true : false;

	if (active)
	{
		if (!dtorque)
		{
			// set limits first.
			// search the max and min.
			// determine the scale factor.
			double min, max;

			// LATER: add the other joints, now it plots only the left arm.
			// this is rather specific.
			for (int i = TRENDMIN; i <= TRENDMAX; i++)
			{
				if (arm->GetTCalibratedFlag(i))
				{
					arm->GetLimits (i, &min, &max);
					scalefactor[i] = (max-min) / 65536.0;
				}
				else
					scalefactor[i] = 0.0;
			}
		}
		else
		{
			double min = -300.0, max = 300.0;

			// LATER: add the other joints, now it plots only the left arm.
			// this is rather specific.
			for (int i = TRENDMIN; i <= TRENDMAX; i++)
			{
				if (arm->GetTCalibratedFlag(i))
				{
					scalefactor[i] = (max-min) / 65536.0;
				}
				else
					scalefactor[i] = 0.0;
			}
		}

		PtSetArg (&arg[0], Pt_ARG_TIMER_INITIAL, 100, 0);
		PtSetArg (&arg[1], Pt_ARG_TIMER_REPEAT, 40, 0);
		PtSetResources (ABW_base_trend_timer, 2, arg);
	}
	else
	{
		PtSetArg (&arg[0], Pt_ARG_TIMER_INITIAL, 0, 0);
		PtSetArg (&arg[1], Pt_ARG_TIMER_REPEAT, 0, 0);
		PtSetResources (ABW_base_trend_timer, 2, arg);

		for (int i = TRENDMIN; i <= TRENDMAX; i++)
			scalefactor[i] = 0;
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_dtorque( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// toggle timer on/off and read and set limits of trend widget.
	PtArg_t arg[1];
	PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
	PtGetResources (ABW_base_trend_toggle, 1, &arg[0]);
	bool active = ((arg[0].value & Pt_SET) != 0) ? true : false;

	PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
	PtGetResources (widget, 1, &arg[0]);
	
	dtorque = ((arg[0].value & Pt_SET) != 0) ? true : false;

	if (active)
	{
		if (!dtorque)
		{
			// set limits first.
			// search the max and min.
			// determine the scale factor.
			double min, max;

			// LATER: add the other joints, now it plots only the left arm.
			// this is rather specific.
			for (int i = TRENDMIN; i <= TRENDMAX; i++)
			{
				if (arm->GetTCalibratedFlag(i))
				{
					arm->GetLimits (i, &min, &max);
					scalefactor[i] = (max-min) / 65536.0;
				}
				else
					scalefactor[i] = 0.0;
			}
		}
		else
		{
			double min = -300.0, max = 300.0;

			// LATER: add the other joints, now it plots only the left arm.
			// this is rather specific.
			for (int i = TRENDMIN; i <= TRENDMAX; i++)
			{
				if (arm->GetTCalibratedFlag(i))
				{
					scalefactor[i] = (max-min) / 65536.0;
				}
				else
					scalefactor[i] = 0.0;
			}
		}
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

