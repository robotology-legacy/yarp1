/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.13X */

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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"


int
exit_callback( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  // cleanup shared memory
  PgShmemCleanup();
  PtAppRemoveSignal(NULL, NULL,
		    (PtSignalProcF_t) ExitSignalHandler, NULL);
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  exit(EXIT_SUCCESS);
  return( Pt_CONTINUE );
  
}

