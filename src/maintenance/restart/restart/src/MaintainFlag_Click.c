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

int maintainFlag = 0;

int
MaintainFlag_Click( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  PtArg_t args[1]; 
  short *num;

  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, &num, NULL);
  PtGetResources(ABW_MaintainFlag, 1, args);
  maintainFlag = *num;
  printf("Flag is %d\n", maintainFlag);
  fflush(stdout);
  
  return( Pt_CONTINUE );
  
}

