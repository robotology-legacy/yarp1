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
#include "headcontrol.h"
#include "abimport.h"
#include "proto.h"

// 
double scalefactor[8] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
short trenddata[8];
bool plot_commands = false;

int
realized( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// assumes there are 7 joints.
	int trend_color_array[7] = 
				  {	
				    Pg_WHITE, 
					Pg_MAGENTA, 
					Pg_MAGENTA, 
					Pg_GREEN, 
					Pg_RED, 
					Pg_YELLOW, 
					Pg_BLUE
				  };

	RtTrendAttr_t one_attr, several_attr[7];
	PtArg_t args[2];

	/* Set up the color list. */
	PtSetArg (&args[0], Rt_ARG_TREND_COLOR_LIST, trend_color_array, 7);
	PtSetResources (widget, 1, args);

	/* Map the trends to colors. */
	for (int i = 0; i < 7; i++)
		several_attr[i].map = i+1;

	PtSetArg (&args[0], Rt_ARG_TREND_ATTRIBUTES, 
		      several_attr, 0);
	PtSetResources (widget, 1, args);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
timer_activate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// read new data, prepare array and send to trend widget.
	// ABW_base_trend

	if (!plot_commands)
		head->GetActualPosition (encoder);
	else
		thread->GetLastControlValue(encoder);
		
	for (int i = 0; i < head->GetNJoints(); i++)
	{
		if (scalefactor[i] != 0.0)
		{
			encoder[i] /= scalefactor[i];
		}
		else
		{
			encoder[i] = 0;
		}

		if (encoder[i] > 32767)
			trenddata[i] = 32767;
		else
		if (encoder[i] < -32768)
			trenddata[i] = -32768;
		else
			trenddata[i] = short(encoder[i]);
	}

	PtArg_t arg;
	PtSetArg (&arg, Rt_ARG_TREND_DATA, trenddata, head->GetNJoints());
	PtSetResources (ABW_base_trend, 1, &arg);		

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
	PtGetResources (ABW_base_commands, 1, &arg[0]);
	plot_commands = arg[0].value & Pt_SET; 

	PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
	PtGetResources (widget, 1, &arg[0]);

	if (!plot_commands)
	{
		if ((arg[0].value & Pt_SET) != 0)
		{
			// set limits first.
			// search the max and min.
			// determine the scale factor. 		
			double min, max;

			for (int i = 0; i < head->GetNJoints(); i++)
			{
				if (head->IsCalibrated(i))
				{
					head->GetLimits (i, &min, &max);
					scalefactor[i] = (max-min) / 65536.0;
				}
				else
					scalefactor[i] = 0.0;
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

			for (int i = 0; i < head->GetNJoints(); i++)
				scalefactor[i] = 0;
		}
	}
	else
	{
		if ((arg[0].value & Pt_SET) != 0)
		{
			// set limits first.
			// search the max and min.
			// determine the scale factor. 		
			double min, max;
			scalefactor[0] = 40000.0/65000.0;
			scalefactor[1] = 180000.0/65000.0;
			scalefactor[2] = 180000.0/65000.0;
			scalefactor[3] = 14000.0/65000.0;
			scalefactor[4] = 14000.0/65000.0;
			scalefactor[5] = 30000.0/65000.0;
			scalefactor[6] = 14000.0/65000.0;

			PtSetArg (&arg[0], Pt_ARG_TIMER_INITIAL, 100, 0);
			PtSetArg (&arg[1], Pt_ARG_TIMER_REPEAT, 40, 0);
			PtSetResources (ABW_base_trend_timer, 2, arg);
		}
		else
		{
			PtSetArg (&arg[0], Pt_ARG_TIMER_INITIAL, 0, 0);
			PtSetArg (&arg[1], Pt_ARG_TIMER_REPEAT, 0, 0);
			PtSetResources (ABW_base_trend_timer, 2, arg);

			for (int i = 0; i < head->GetNJoints(); i++)
				scalefactor[i] = 0;
		}
	}
		
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

