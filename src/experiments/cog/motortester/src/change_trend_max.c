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
change_trend_max( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  short int tmax;
  PtNumericIntegerCallback_t *cb;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  cb = cbinfo->cbdata;
  tmax = cb->numeric_value;

  // printf("max set to %d\n", tmax);
  
  PtSetArg(&args[0], Rt_ARG_TREND_MAX, tmax, 1);
  PtSetResources(ABW_position_trend, 1, args);

  return( Pt_CONTINUE );
}

