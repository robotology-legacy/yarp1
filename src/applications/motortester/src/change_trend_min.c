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
#include "motortester.h"
#include "abimport.h"
#include "proto.h"


int
change_trend_min( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  short int tmin;
  PtNumericIntegerCallback_t *cb;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  cb = cbinfo->cbdata;
  tmin = cb->numeric_value;

  // printf("min set to %d\n", tmin);

  PtSetArg(&args[0], Rt_ARG_TREND_MIN, tmin, 1);
  PtSetResources(ABW_position_trend, 1, args);

  return( Pt_CONTINUE );
}
