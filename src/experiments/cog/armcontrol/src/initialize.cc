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

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */


YARPArmControl 	 *arm = NULL;
CCommandReceiver *thread = NULL;
CEncoderThread   *ethread = NULL;
CTorsoController *tthread = NULL;
double 	         *actual_pos = NULL;
double           *zero_pos = NULL;

int
initialize( int argc, char *argv[] )
{
	int ret = qnx_scheduler( 0, 0, SCHED_RR, -1, 1 );
	if (ret < 0)
			cout << "cannot set new scheduler policy" << endl;

	arm = new YARPArmControl;
	assert (arm != NULL);

	arm->Initialize ();

	actual_pos = new double[arm->GetNJoints()];
	assert (actual_pos != NULL);
	zero_pos = new double[arm->GetNJoints()];
	assert (zero_pos != NULL);

	arm->GetPositionZeros (zero_pos);

	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	return( Pt_CONTINUE );
}

int
startall (void)
{
	thread = new CCommandReceiver;
	assert (thread != NULL);
	ethread = new CEncoderThread;
	assert (ethread != NULL);
	tthread = new CTorsoController;
	assert (tthread != NULL);
	YARPTime::DelayInSeconds (1.0);

	printf ("Arm control is started - MEI reset\n");
	printf ("Starting low-level threads\n");

	arm->StartPositionControl (1);
	thread->Start (1);
	ethread->Start (1);
	tthread->Start (1);
	printf ("Threads started\n");

	printf ("BEFORE calibrating ensure the torso is OFF\nYou can turn it on after the calibration is completed\n");

	fflush (stdout);

	printf ("Set alpha filter to 0.3\n");
	arm->SetAlphaFilter (0.3);

	//
	//
	// initialize widgets.
	double a;
	arm->GetAlphaFilter (&a);
	PtArg_t arg;
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &a, 0);
	PtSetResources (ABW_base_alpha_value, 1, &arg);

	int jnt = read_integer_from_widget (ABW_base_axis_number);
	if (jnt < LEFTMINAXIS || jnt > LEFTMAXAXIS)
		return 0;

	int16 prop, der, shift;
	arm->GetMEIGain (jnt, DF_SHIFT, &shift);
	arm->GetMEIGain (jnt, DF_P, &prop);
	arm->GetMEIGain (jnt, DF_D, &der);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, shift, 0);
	PtSetResources (ABW_base_gain_shift, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, prop, 0);
	PtSetResources (ABW_base_gain_mprop, 1, &arg);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, der, 0);
	PtSetResources (ABW_base_gain_mder, 1, &arg);

	double p, d;

	arm->GetPositionGain (jnt, PROPORTIONAL, &p);
	arm->GetPositionGain (jnt, DERIVATIVE, &d);

	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &p, 0);
	PtSetResources (ABW_base_gain_pprop, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &d, 0);
	PtSetResources (ABW_base_gain_pder, 1, &arg);

	//
	// initialize torso widgets.
	//
	tthread->GetGain (p, d);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &p, 0);
	PtSetResources (ABW_base_torso_pprop, 1, &arg);
	PtSetArg (&arg, Pt_ARG_NUMERIC_VALUE, &d, 0);
	PtSetResources (ABW_base_torso_pder, 1, &arg);

	return 0;
}

int
quit_application( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (thread != NULL)
	{
		thread->Terminate (1);
		delete thread;
		thread = NULL;
	}

	if (ethread != NULL)
	{
		ethread->Terminate (1);
		delete ethread;
		ethread = NULL;
	}

	if (tthread != NULL)
	{
		tthread->Terminate (1);
		delete tthread;
		tthread = NULL;
	}

	arm->StopPositionControl (1);
	arm->Uninitialize ();

	if (actual_pos != NULL)
		delete[] actual_pos;
	if (zero_pos != NULL)
		delete[] zero_pos;

	YARPThread::TerminateAll();
	YARPTime::DelayInSeconds(0.25);

	exit (0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

