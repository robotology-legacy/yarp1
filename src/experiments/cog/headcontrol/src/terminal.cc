/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Toolkit headers */
#include <Ph.h>
#include <Pt.h>
#include <Ap.h>

/* Local headers */
#include "headcontrol.h"
#include "abimport.h"
#include "proto.h"


int
realize( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	PtArg_t arg;
	PtSetArg(&arg, Pt_ARG_TTY_PSEUDO, NULL, 0);
	PtSetResources(widget , 1, &arg );

	/* Have we succeeded? */
	PtSetArg( &arg, Pt_ARG_TTY_FD, 0, 0 );
	PtGetResources( widget, 1, &arg );

	if ( arg.value == -1 )
	    PtTerminalPuts(widget, "Unable to find a pty\r\n");

	PtSetArg (&arg, Pt_ARG_TTY_FD, NULL, 0);
	PtGetResources(widget, 1, &arg);	

	int ret = dup2 (arg.value, 1);
	if (ret != 1)
	   PtTerminalPuts(widget, "Unable to redirect stdout\r\n");

	// from now on printf is ok.

	PtSetArg (&arg, Pt_ARG_TTY_PSEUDO, NULL, 0);
	PtGetResources(widget, 1, &arg);	
	printf ("Terminal is : %s\n", arg.value);	

	startall ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );

	}

