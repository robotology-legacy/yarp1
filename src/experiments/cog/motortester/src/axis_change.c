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
axis_change( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  unsigned short *newaxis;
  
  // index is Pt_ARG_CBOX_SEL_ITEM
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newaxis, 0 ); 
  PtGetResources(ABW_axis_selector, 1, args);
  current_axis = *newaxis - 1;
  // fprintf(stderr, "New AXIS : %d\n", current_axis);

  draw_gains();
  return( Pt_CONTINUE );
  
}

