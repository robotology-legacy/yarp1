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
zero_encoder( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  int stat;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  /*stat = meid_zero_axis(meidx, current_axis);*/
  stat = meid_set_position (meidx, current_axis, 0.0);
  if (stat != 0)
    {
      sprintf(errmsg, "MEI Zero Axis... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "zero_encoder", errmsg,
	      __FILE__);
      exit(-1);
    }

  return( Pt_CONTINUE );

}

