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

int enable_freeze = 0;

char secondary_buffer[FRAME_HEIGHT*FRAME_WIDTH*3*MAX_SCALE*MAX_SCALE];

void set_freeze()
{
  PtArg_t args[1];
  
  enable_freeze = 1;
  if (ImageLoaded)
    {
      memcpy(secondary_buffer, ind, FRAME_WIDTH*FRAME_HEIGHT*3*image_scale*image_scale);
    }
  else
    memcpy(secondary_buffer, ind, FRAME_WIDTH*FRAME_HEIGHT*3);
  ind = secondary_buffer;
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, 1, 0);
  PtSetResources(ABW_freeze_button, 1, args);
}

int
freeze( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int button_state;
  PtOnOffButtonCallback_t *cback;
  
  cback = (PtOnOffButtonCallback_t *) cbinfo->cbdata;
  button_state = cback->state;
  
  enable_freeze = button_state;
  
  if (enable_freeze)
    set_freeze();
  else
    ImageLoaded = false;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
  
}

