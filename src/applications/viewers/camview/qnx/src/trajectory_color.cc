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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"

int color_mode = 0;

int
trajectory_color( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  unsigned short *newmode;
  PtArg_t args[1];
  
  PtSetArg( &args[0], Pt_ARG_CBOX_SEL_ITEM, &newmode, 0);
  PtGetResources( ABW_trajectory_combo, 1, args);
  
  color_mode = *newmode - 1;
  
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
  
}
