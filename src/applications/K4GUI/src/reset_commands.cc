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
#include "mycommands.h"

extern YAPOCMessage newcommand;

/* this function is called when users click on the Reset PIC button which is
   a request to reset JRKERR boards */
int
ResetPIC( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	//CreateMessage(PIC_RESET);
	newcommand.CreatePICResetMsg();
	  SendMessage(newcommand);
        printf("Sending request to reset PIC\n");
	return( Pt_CONTINUE );

	}

/* this function is called when users click on the Reset MEID button */
int
MEIDreset( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	/* eliminate 'unreferenced' warnings */
	  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	  //CreateMessage(MEID_RESET);
	  newcommand.CreateMEIDResetMsg();
	  SendMessage(newcommand);
	  printf("Sending request to reset MEID\n");
	  
	  
	  return( Pt_CONTINUE );

	}



