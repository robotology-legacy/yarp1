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
#include "gyrotester.h"
#include "abimport.h"
#include "proto.h"


int
unboresight( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int stat;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  stat = isd_unboresight(isdx);
  if (stat != 0)
    {
      sprintf(errmsg, "Unboresight command failed... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "unboresight", errmsg,
	      __FILE__);
    }
  
  return( Pt_CONTINUE );
  
}

