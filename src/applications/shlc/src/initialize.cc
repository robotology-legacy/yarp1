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
#include "shlc.h"
#include "abimport.h"
#include "proto.h"

#include "classes.h"

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */


/* Globals */
InputManager	*manager;
ImageReader		*reader;
MessageReceiver *messenger;

char genericbuf[512];

bool play_from_memory = false;
bool opticflow_mode = false;
bool secret_mode = false;
bool canonical_mode = false;

/* for playing from memory store */
int playing_frame = 0;

YARPOutofScreenImage *oos = NULL;
YARPOutofScreenImage *oosd = NULL;

YARPOutofScreenImage **oosi = NULL;
YARPOutofScreenImage **ooso = NULL;

PlotVectors *plot_vectors = NULL;
PlotVectors *plot_vectors2 = NULL;

char globaldirname[512];

YARPVelocityField *velocity_field = NULL;
YARPImageOf<YarpPixelMono> *mono = NULL;
YARPImageOf<YarpPixelBGR> *output = NULL;

//
//
//
int
initialize( int argc, char *argv[] )
{
	reader = new ImageReader (WIDTH, HEIGHT);
	assert (reader != NULL);

	reader->Start();

	messenger = new MessageReceiver ();
	assert (messenger != NULL);

	messenger->SetReader (reader);
	messenger->Start ();

	reader->SetMessageReceiver (messenger);

	memset (genericbuf, 0, 512);
	memset (globaldirname, 0, 512);

	play_from_memory = false;
	playing_frame = 0;

	oos = new YARPOutofScreenImage(WIDTH, HEIGHT);
	assert (oos != NULL);

	oosd = new YARPOutofScreenImage(WIDTH, HEIGHT);
	assert (oosd != NULL);

	oosi = new YARPOutofScreenImage* [6];
	assert (oosi != NULL);
	for (int i = 0; i < 6; i++)
	{
		oosi[i] = new YARPOutofScreenImage(WIDTH, HEIGHT);
		assert (oosi[i] != NULL);
	}

	ooso = new YARPOutofScreenImage* [6];
	assert (ooso != NULL);
	for (i = 0; i < 6; i++)
	{
		ooso[i] = new YARPOutofScreenImage(WIDTH, HEIGHT);
		assert (ooso[i] != NULL);
	}


	// to plot the arm position.
	plot_vectors = new PlotVectors (FRAMES2STORE, NumArmJoints, 4, 9);
	assert (plot_vectors != NULL);
	plot_vectors->SetDataPtr (reader->GetSequenceRef().GetArmPositionPtr());

	// to plot the head position.	
	plot_vectors2 = new PlotVectors (FRAMES2STORE, NumHeadJoints, 1, 7);
	assert (plot_vectors2 != NULL);
	plot_vectors2->SetDataPtr (reader->GetSequenceRef().GetHeadPositionPtr());


	// optic flow computation.
	velocity_field = new YARPVelocityField (WIDTH, HEIGHT);
	assert (velocity_field != NULL);
	mono = new YARPImageOf<YarpPixelMono>;
	assert (mono != NULL);
	mono->Resize(WIDTH, HEIGHT);

	output = new YARPImageOf<YarpPixelBGR>;
	assert (output != NULL);
	output->Resize(WIDTH, HEIGHT);

	alloc_secret ();

	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	return( Pt_CONTINUE );
}

int
realize_display( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	manager = new InputManager (WIDTH, HEIGHT);
	assert (manager != NULL);

	manager->SetWidget (widget);
	manager->SetChannel (reader);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
unrealize_display( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	//delete manager;

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
quit( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	// disable the timer.
	PtArg_t args[3];
	PtSetArg( &args[0], Pt_ARG_TIMER_REPEAT, 0, 0 );
	PtSetArg( &args[1], Pt_CB_TIMER_ACTIVATE, NULL, 0);
	PtSetResources(ABW_base_display_timer, 2, args);

	// disconnect out ports.
	YARPPort::DeactivateAll();

	// disconnect in ports.
	spawnlp (P_NOWAIT, DISCONNECT_SCRIPT_NAME, DISCONNECT_SCRIPT_NAME, NULL);

	YARPTime::DelayInSeconds(1);

	free_secret ();

	delete manager;

	reader->Terminate();
	delete reader;

	messenger->Terminate();
	delete messenger;

	if (oos != NULL) delete oos;
	if (oosd != NULL) delete oosd;

	if (oosi != NULL)
	{
		for (int i = 0; i < 6; i++)
			delete oosi[i];

		delete[] oosi;
	}

	if (ooso != NULL)
	{
		for (int i = 0; i < 6; i++)
			delete ooso[i];

		delete[] ooso;
	}

	if (plot_vectors != NULL) delete plot_vectors;
	if (plot_vectors2 != NULL) delete plot_vectors2;

	if (velocity_field != NULL) delete velocity_field;
	if (mono != NULL) delete mono;
	if (output != NULL) delete output;

	exit(0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
display_timer_activate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (!play_from_memory) 
		manager->Display ();

	//
	if (reader->SafeAccess())
	{
		sprintf (genericbuf, "Frames in storage = %4d\0", reader->GetSequenceRef().GetNumElements());	

		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
		PtSetResources (ABW_base_frame_stored, 1, &arg[0]);		
	}

	// check whether is fine, uptodate.
	if (reader->GetOutputSequenceRef().UpToDate())
	{
		sprintf (genericbuf, "Output is up to date\0");	
		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
		PtSetResources (ABW_base_uptodate, 1, &arg[0]);		
	}
	else
	{
		sprintf (genericbuf, "Output is out of date\0");	
		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
		PtSetResources (ABW_base_uptodate, 1, &arg[0]);		
	}

	if (opticflow_mode)
	{
		sprintf (genericbuf, "On\0");	
		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
		PtSetResources (ABW_base_opticflow_on, 1, &arg[0]);		
	}
	else
	{
		sprintf (genericbuf, "Off\0");	
		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
		PtSetResources (ABW_base_opticflow_on, 1, &arg[0]);		
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

