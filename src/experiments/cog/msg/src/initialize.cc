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
#include "abimport.h"
#include "proto.h"

//
#include "middlelevel.h"
#include "classes.h"

#include "process.h"

/* Application Options string */
const char ApOptions[] =
	AB_OPTIONS ""; /* Add your options in the "" */

ReachingThread thread;

int
initialize( int argc, char *argv[] )
{
//	__debug_level = 82;

	thread.Start ();

	/* eliminate 'unreferenced' warnings */
	argc = argc, argv = argv;

	return( Pt_CONTINUE );
}


int
quit( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	spawnlp (P_NOWAIT, DISCONNECTSCRIPTNAME, DISCONNECTSCRIPTNAME, NULL);
	YARPTime::DelayInSeconds(1);

	thread.Terminate ();

	exit (0);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

