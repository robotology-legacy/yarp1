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

/* this function is called when users change max value for trend 5*/
int
Trend5MaxChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  
            PtArg_t args[1];
            short int tmax;
            PtNumericIntegerCallback_t *cb;
  
            /* eliminate 'unreferenced' warnings */
            widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
            cb = (PtNumericIntegerCallback *)cbinfo->cbdata;
            tmax = cb->numeric_value;
  
            // printf("max set to %d\n", tmax);
  
            PtSetArg(&args[0], Rt_ARG_TREND_MAX, tmax, 1);
            PtSetResources(ABW_Trend5, 1, args);
            return( Pt_CONTINUE );
  
}

/* this function is called when users change min value for trend 5*/
int
Trend5MinChange( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
            PtArg_t args[1];
            short int tmin;
            PtNumericIntegerCallback_t *cb;
  
            /* eliminate 'unreferenced' warnings */
            widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
            cb = (PtNumericIntegerCallback *)cbinfo->cbdata;
            tmin = cb->numeric_value;
  
            // printf("min set to %d\n", tmin);
  
            PtSetArg(&args[0], Rt_ARG_TREND_MIN, tmin, 1);
            PtSetResources(ABW_Trend5, 1, args);
            return( Pt_CONTINUE );
  
}
