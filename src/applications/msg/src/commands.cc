/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <process.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "middlelevel.h"
#include "abimport.h"
#include "proto.h"

HeadMessage hmsg_array[10];
ArmMessage  amsg_array[10];
HeadMessage& msg = hmsg_array[0];
ArmMessage& amsg = amsg_array[0];

int
connect( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{        
	int ret = spawnlp (P_NOWAIT, CONNECTSCRIPTNAME, CONNECTSCRIPTNAME, NULL);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

//
//
// idle mode callbacks.
//

int
raise_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgMoveToOld;
	amsg.j[8] = 90+541;
	amsg.j[7] = -90+680;
	amsg.j[6] = -150+704;
	amsg.j[5] = -70+415;
	amsg.j[4] = 20+240;
	amsg.j[3] = 90+294;

	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
relax_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgMoveToOld;
	amsg.j[3] = 294;
	amsg.j[4] = 240;
	amsg.j[5] = 415;
	amsg.j[6] = 704;
	amsg.j[7] = 680;
	amsg.j[8] = 541;

	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
wave_arm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int shouldera = read_integer_from_widget (ABW_base_shouldera);
	int shoulderb = read_integer_from_widget (ABW_base_shoulderb);
	int elbowa = read_integer_from_widget (ABW_base_elbowa);
	int elbowb = read_integer_from_widget (ABW_base_elbowb);
	int wrista = read_integer_from_widget (ABW_base_wrista);
	int wristb = read_integer_from_widget (ABW_base_wristb);

	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgWaveHand;
	amsg.j[8] = double(wristb);
	amsg.j[7] = double(wrista);
	amsg.j[6] = double(elbowb);
	amsg.j[5] = double(elbowa);
	amsg.j[4] = double(shoulderb);
	amsg.j[3] = double(shouldera);

	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
stop_neck( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&msg, 0, sizeof(HeadMessage));
	msg.type = HeadMsgStopNeckMovement;

	thread.SendHeadMessage (&msg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
look_down( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&msg, 0, sizeof(HeadMessage));
	msg.type = HeadMsgMoveToPosition;
	msg.j1 = -10000;
	msg.j2 = 0;
	msg.j3 = 0;
	msg.j4 = 10000;
	msg.j5 = 0;
	msg.j6 = 0;
	msg.j7 = -5000;

	thread.SendHeadMessage (&msg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
clear_headmsg( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	msg.type = 0;
	thread.SendHeadMessage (&msg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
do_headarm( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgMoveToOld;
	amsg.j[8] = 541;
	amsg.j[7] = 680;
	amsg.j[6] = -127+704;
	amsg.j[5] = -171+415;
	amsg.j[4] = 240;
	amsg.j[3] = 123+294;

	memset (&msg, 0, sizeof(HeadMessage));
	msg.type = HeadMsgMoveToPosition;
	msg.j1 = -10000;
	msg.j2 = 0;
	msg.j3 = 0;
	msg.j4 = 10000;
	msg.j5 = 0;
	msg.j6 = 0;
	msg.j7 = -6000;

	thread.SendMessageSequence (amsg_array, 1, hmsg_array, 1);
	YARPTime::DelayInSeconds(0.04);

	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgWaveHand;
	amsg.j[8] = 541;
	amsg.j[7] = 680;
	amsg.j[6] = 704;
	amsg.j[5] = 150+415;
	amsg.j[4] = 240;
	amsg.j[3] = 294;
	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
clear_all( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgMoveToOld;
	amsg.j[8] = 541;
	amsg.j[7] = 680;
	amsg.j[6] = 704;
	amsg.j[5] = 415;
	amsg.j[4] = 240;
	amsg.j[3] = 294;

	thread.SendArmMessage (&amsg);
	YARPTime::DelayInSeconds (0.04);  // need to wait a thread period.

	msg.type = 0;
	thread.SendHeadMessage (&msg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

//
// position control from GUI.
//

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
arm_goto( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int shouldera = read_integer_from_widget (ABW_base_shouldera);
	int shoulderb = read_integer_from_widget (ABW_base_shoulderb);
	int elbowa = read_integer_from_widget (ABW_base_elbowa);
	int elbowb = read_integer_from_widget (ABW_base_elbowb);
	int wrista = read_integer_from_widget (ABW_base_wrista);
	int wristb = read_integer_from_widget (ABW_base_wristb);

	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgMoveToOld;
	amsg.j[8] = double(wristb);
	amsg.j[7] = double(wrista);
	amsg.j[6] = double(elbowb);
	amsg.j[5] = double(elbowa);
	amsg.j[4] = double(shoulderb);
	amsg.j[3] = double(shouldera);
	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
head_goto( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int pan = read_integer_from_widget (ABW_base_pan);
	int tilt = read_integer_from_widget (ABW_base_tilt);
	int vergence = read_integer_from_widget (ABW_base_vergence);

	memset (&msg, 0, sizeof(HeadMessage));
	msg.type = HeadMsgMoveToOrientation;
	msg.j1 = pan * degToRad;
	msg.j2 = tilt * degToRad;
	msg.j7 = vergence * degToRad;
	thread.SendHeadMessage (&msg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

//
// change mode callbacks.
//

int
start_reaching( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StartReaching ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
stop_reaching( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StopReaching ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
start_planar( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StartPlanar ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

//
// std reaching callbacks.
//

int
toggle_wave( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	thread.ToggleWaveArm();

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
setnoise( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	double noise = read_double_from_widget (ABW_base_noiselevel);
	thread.SetNoiseLevel (noise);

	return( Pt_CONTINUE );
}

//
// Planar mode control callbacks.
//
int
set_intermediate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StoreSidePosition ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
set_basis1( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StoreBasisPosition (0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
set_basis2( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StoreBasisPosition (1);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
set_basis3( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StoreBasisPosition (2);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
savecalibration( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.StoreCalibration ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
loadcalibration( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.LoadCalibration ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
stepforward( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.Mode3();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
toggledebug( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.ToggleDebugMode3();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
uncalibrate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.Uncalibrate();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
move_torso( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int torsoyaw = read_integer_from_widget (ABW_base_torso_yaw);

	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgTorsoPosition;
	amsg.j[2] = double(torsoyaw);
	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
torso_reset( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	memset (&amsg, 0, sizeof(ArmMessage));
	amsg.type = ArmMsgTorsoLink2Head;
	thread.SendArmMessage (&amsg);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_wave3( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	thread.ToggleWaveArm3();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

