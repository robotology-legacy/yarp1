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

void set_trigger(int update_button_state)
{
  PtArg_t args[1];
  PhDim_t *trend_dimensions;
  unsigned short w;
  
  trigger_enabled = TRUE;
  trigger_fired = FALSE;
  PtSetArg(&args[0], Pt_ARG_DIM, &trend_dimensions, 0);
  PtGetResources(ABW_position_trend, 1, args);
  w = trend_dimensions->w;

  // printf("width = %d  height = %d\n", w, trend_dimensions->h);
  trigger_time = w/3;
  
  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, trigger_enabled, 0);
      PtSetResources(ABW_trigger_switch, 1, args);
    }
}

void remove_trigger(int update_button_state)
{
  PtArg_t args[1];

  trigger_enabled = FALSE;
  trigger_fired = FALSE;
  trigger_time = 0;

  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, trigger_enabled, 0);
      PtSetResources(ABW_trigger_switch, 1, args);
    }
}

int
enable_trigger( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtOnOffButtonCallback_t *cback;
  int button_state;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  /* Update the global link state variable */
  cback = cbinfo->cbdata;
  button_state = cback->state;

  if (button_state != 0) {
    set_trigger(FALSE);
  } else {
    remove_trigger(FALSE);
  }
    
  return( Pt_CONTINUE );
  
}

