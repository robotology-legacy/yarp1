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
change_sample_rate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  PtNumericIntegerCallback_t *cb;
  int timer_rate;
  long timer_rate_long;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  cb = cbinfo->cbdata;
  timer_rate = cb->numeric_value;
  timer_rate_long = (long) timer_rate;
  
  PtSetArg(&args[0], Pt_ARG_TIMER_REPEAT, timer_rate_long, 0);
  PtSetResources(ABW_global_timer, 1, args);

  return( Pt_CONTINUE );
  
}

