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


double
read_double_from_widget(PtWidget_t *widget)
{
	PtArg_t args[1];
	double *dval;

	PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &dval, 0);
	PtGetResources(widget, 1, args);
	return(*dval);
}

int
read_integer_from_widget(PtWidget_t *widget)
{
	PtArg_t args[1];
	int *intval;

	PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &intval, 0);
	PtGetResources(widget, 1, args);
	return(*intval);
}

//
// actually calibrates all the enabled joints.
int
calibrate_left( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	printf ("calibrating the left arm...\n");

	for (int j = LEFTMINAXIS; j <= LEFTMAXAXIS; j++)
	{
		arm->TorqueCalibration (j);
		arm->TrustDefaultLimits (j);
		arm->EnableAxis (j);
	}

	// enable torque readings.
	// Doesn't work. Too slow.
	//arm->EnableTorque ();

	int jnt = read_integer_from_widget (ABW_base_axis_number);
	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return (Pt_CONTINUE);

	int16 prop, der, shift;
	arm->GetMEIGain (jnt, DF_SHIFT, &shift);
	arm->GetMEIGain (jnt, DF_P, &prop);
	arm->GetMEIGain (jnt, DF_D, &der);

	PtArg_t arg;
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, shift, 0);
	PtSetResources (ABW_base_gain_shift, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, prop, 0);
	PtSetResources (ABW_base_gain_mprop, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, der, 0);
	PtSetResources (ABW_base_gain_mder, 1, &arg);

	double p,d,i,l;

	arm->GetPositionGain (jnt, PROPORTIONAL, &p);
	arm->GetPositionGain (jnt, DERIVATIVE, &d);
	arm->GetPositionGain (jnt, INTEGRAL, &i);
	arm->GetPositionGain (jnt, INTEGRAL_LIMIT, &l);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &p, 0);
	PtSetResources (ABW_base_gain_pprop, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &d, 0);
	PtSetResources (ABW_base_gain_pder, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &i, 0);
	PtSetResources (ABW_base_gain_pint, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &l, 0);
	PtSetResources (ABW_base_gain_plim, 1, &arg);

	// set also the current position.
	double pos = 0;
	arm->GetPosition (jnt, &pos);

	int ipos = int (pos+.5);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, ipos, 0);
	PtSetResources (ABW_base_duration, 1, &arg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
raise_gain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	printf ("bringing gains UP...\n");
	arm->RaisePositionGains();
//	arm->RaiseMEIGains();
	printf ("gains are UP\n");

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
zero_gain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	printf ("bringing gains DOWN...\n");
	arm->LowerPositionGains();
//	arm->LowerMEIGains();
	printf ("gains are DOWN...\n");

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_time( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	arm->ToggleTiming ();	

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
print_position( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	arm->GetPositions(actual_pos);

	printf ("position\n");
	for (int i = LEFTMINAXIS; i <= LEFTMAXAXIS; i++)
	{
		printf ("%d : %f\n", i, actual_pos[i]);	
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
set_alpha( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	double a = read_double_from_widget (ABW_base_alpha_value);
	arm->SetAlphaFilter (a);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
print_axis( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int jnt = read_integer_from_widget (ABW_base_axis_number);
  
	if (jnt >= LEFTMINAXIS && jnt <= LEFTMAXAXIS) 
		arm->PrintAxis (jnt);
	else
		printf ("axis %d not supported yet\n", jnt);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
do_test_motion( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int jnt = read_integer_from_widget (ABW_base_axis_number);

	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return (Pt_CONTINUE);
	
	int speed = read_integer_from_widget (ABW_base_speed);
	int pos = read_integer_from_widget (ABW_base_duration);

	arm->SetSpeedSetpoint (jnt, speed);
	arm->SetAccelSetpoint (jnt, speed * 10);
	arm->MoveToSimple (jnt, pos);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
axis_changed( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int jnt = read_integer_from_widget (widget);
	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return (Pt_CONTINUE);

	int16 prop, der, shift;
	arm->GetMEIGain (jnt, DF_SHIFT, &shift);
	arm->GetMEIGain (jnt, DF_P, &prop);
	arm->GetMEIGain (jnt, DF_D, &der);

	PtArg_t arg;
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, shift, 0);
	PtSetResources (ABW_base_gain_shift, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, prop, 0);
	PtSetResources (ABW_base_gain_mprop, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, der, 0);
	PtSetResources (ABW_base_gain_mder, 1, &arg);

	double p,d,i,l;

	arm->GetPositionGain (jnt, PROPORTIONAL, &p);
	arm->GetPositionGain (jnt, DERIVATIVE, &d);
	arm->GetPositionGain (jnt, INTEGRAL, &i);
	arm->GetPositionGain (jnt, INTEGRAL_LIMIT, &l);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &p, 0);
	PtSetResources (ABW_base_gain_pprop, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &d, 0);
	PtSetResources (ABW_base_gain_pder, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &i, 0);
	PtSetResources (ABW_base_gain_pint, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &l, 0);
	PtSetResources (ABW_base_gain_plim, 1, &arg);

	// set also the current position.
	double pos = 0;
	arm->GetPosition (jnt, &pos);

	int ipos = int (pos+.5);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, ipos, 0);
	PtSetResources (ABW_base_duration, 1, &arg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
set_tgain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int jnt = read_integer_from_widget (ABW_base_axis_number);
	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return (Pt_CONTINUE);

	int prop, der, shift;
	shift = read_integer_from_widget (ABW_base_gain_shift);
	prop = read_integer_from_widget (ABW_base_gain_mprop);
	der = read_integer_from_widget (ABW_base_gain_mder);

	arm->SetMEIGain (jnt, DF_SHIFT, int16(shift));
	arm->SetMEIGain (jnt, DF_P, int16(prop));
	arm->SetMEIGain (jnt, DF_D, int16(der));

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
set_pgain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int jnt = read_integer_from_widget (ABW_base_axis_number);
	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return (Pt_CONTINUE);

	double p,d,i,l;
	p = read_double_from_widget (ABW_base_gain_pprop);
	d = read_double_from_widget (ABW_base_gain_pder);
	i = read_double_from_widget (ABW_base_gain_pint);
	l = read_double_from_widget (ABW_base_gain_plim);

	arm->SetPositionGain (jnt, PROPORTIONAL, p);
	arm->SetPositionGain (jnt, DERIVATIVE, d);
	arm->SetPositionGain (jnt, INTEGRAL, i);
	arm->SetPositionGain (jnt, INTEGRAL_LIMIT, l);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
gravitysave( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	arm->GravitySaveToFile ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
gravityactivate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	arm->GravityToggleActive();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
gravitylearn( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	arm->GravityToggleLearning ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
torsoraisegain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	printf ("Bringing torso gain up...\n");
	arm->RaiseMEIGains ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
torsozerogain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	printf ("Bringing torso gain down...\n");
	arm->LowerMEIGains ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
settorsogain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	double p,d;
	p = read_double_from_widget (ABW_base_torso_pprop);
	d = read_double_from_widget (ABW_base_torso_pder);
	tthread->SetGain (p, d);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
connectbody( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg;

	PtSetArg( &arg, Pt_ARG_TTY_CMD, CONNECTBODYSCRIPTNAME, 0 );
	PtSetResources( ABW_base_tty, 1, &arg );

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

