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
#include "abimport.h"
#include "proto.h"
#include "motortester.h"

int
reset( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int stat;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  stat = meid_dsp_reset(meidx);
  if (stat != 0)
    {
      sprintf(errmsg, "MEI Reset Failed... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "reset", errmsg,
	      __FILE__);
      return(Pt_CONTINUE);
    }
  
  draw_gains();
  
  return( Pt_CONTINUE );
  
}

