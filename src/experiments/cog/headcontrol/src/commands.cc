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

int
print_encoder( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	head->GetActualPosition (encoder);

    printf ("Eye tilt  : %f\n", encoder[head->EyeTilt()]);
    printf ("Left eye  : %f\n", encoder[head->LeftEye()]);
    printf ("Right eye : %f\n", encoder[head->RightEye()]);
    printf ("Head tilt : %f\n", encoder[head->HeadTilt()]);
    printf ("Head roll : %f\n", encoder[head->HeadRoll()]);
    printf ("Neck pan  : %f\n", encoder[head->NeckPan()]);
    printf ("Neck tilt : %f\n", encoder[head->NeckTilt()]);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
attentionconnectall( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg;

    //int ret = spawnlp (P_WAIT, ATTENTIONCONNECTALL, ATTENTIONCONNECTALL, NULL);
    //PtSetArg( &arg, Pt_ARG_TTY_CMD, ATTENTIONCONNECTALL, 0 );
    PtSetArg( &arg, Pt_ARG_TTY_CMD, ATTN_CONNECT, 0 );
    PtSetResources( ABW_base_tty, 1, &arg );

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
attentionconnectcontrol( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg;

    //int ret = spawnlp (P_WAIT, ATTENTIONCONNECTCONTROL, ATTENTIONCONNECTCONTROL, NULL);
    //PtSetArg( &arg, Pt_ARG_TTY_CMD, ATTENTIONCONNECTCONTROL, 0 );
    PtSetArg( &arg, Pt_ARG_TTY_CMD, CTRL_CONNECT, 0 );
    PtSetResources(ABW_base_tty, 1, &arg );

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
print_limits( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
    double min, max;
    for (int i = 0; i < head->GetNJoints(); i++)
    {
        head->GetLimits (i, &min, &max);
        printf ("joint : %d - min : %f max : %f\n", i, min, max);
    }

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_control( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg;
	PtSetArg (&arg, Pt_ARG_FLAGS, 0, 0);
	PtGetResources (widget, 1, &arg);

	if ((arg.value & Pt_SET) != 0)
	{
		thread->RestartControl();
	}
	else
	{
		thread->GoToNull();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
show_time( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread->ToggleTiming();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
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


int
gotopos( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int *flags;
	int jnt = read_integer_from_widget (ABW_base_joint_number);
	if (jnt < 1 || jnt > head->GetNJoints())
	{
		printf ("please use an axis number between 1 and %d\n", head->GetNJoints());	
	}

	int pos = read_integer_from_widget (ABW_base_encoder_pos);

	if (thread->IsNull())
	{
		flags = new int[head->GetNJoints()];
		memset (flags, 0, sizeof(int) * head->GetNJoints());
		printf ("Moving axis %d to %d\n", jnt, pos);
		head->GetActualPosition (encoder);
		encoder[jnt-1] = pos;
		flags[jnt-1] = 1;
		head->PositionCommand (encoder);
		delete[] flags;
	}	

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


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
set_fb_gain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	double l = read_double_from_widget (ABW_base_eyes_pan_gain);
	double t = read_double_from_widget (ABW_base_eyes_tilt_gain);

	thread->SetFeedbackEyesGain (l, l, t);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
eyes_tilt_gain_realized( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
//	double l,r,t;
//	thread->GetFeedbackEyesGain (&l, &r, &t);
//	printf ("l : %f r : %f t : %f\n", l, r, t);

//	PtArg_t arg;
//	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, l, 0);
//	PtSetResources (widget, 1, &arg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
set_sac_gain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	double p = read_double_from_widget (ABW_base_sac_pan);
	double t = read_double_from_widget (ABW_base_sac_tilt);

	thread->SetSaccadeGain (p, t);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
set_disp_gain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	double g = read_double_from_widget (ABW_base_disp_gain);
	thread->SetDisparityGain (g);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
parkhead( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (thread->IsNull())
	{ 
		head->ParkHead ();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
unparkhead( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (thread->IsNull())
	{ 
		head->UnparkHead ();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
raisegain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (thread->IsNull())
	{
		head->RaiseGains();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
lowergain( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (thread->IsNull())
	{
		head->LowerGains();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
connect_body( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg;

    PtSetArg( &arg, Pt_ARG_TTY_CMD, CONNECTBODYSCRIPTNAME, 0 );
    PtSetResources( ABW_base_tty, 1, &arg );

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

