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
goto_goal( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  int nameindex;
  int goalposition;
  double accel, vel;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  // determine which button was pressed
  nameindex  = ApName(widget);
  // get the appropriate target
  if (nameindex == ABN_goto_goal_1)
    {
      goalposition = read_integer_from_widget(ABW_goal_position_1);
    }
  else if (nameindex == ABN_goto_goal_2)
    {
      // get the appropriate target
      goalposition = read_integer_from_widget(ABW_goal_position_2);
    }
  else
    {
      sprintf(errmsg, "Goto goal can't find the button... \n");
      ApError(ABW_base, NULL, "goto_goal", errmsg,
	      __FILE__);
      return(Pt_CONTINUE);
    }

  // read velocity and acceleration
  vel = read_double_from_widget(ABW_velocity_input);
  accel = read_double_from_widget(ABW_acceleration_input);
  
  if (trigger_enabled)
    trigger_fired = TRUE;

  // send target to mei daemon
  // printf("goal %d vel %d accel %d\n", goalposition, vel, accel);
  meid_start_move(meidx, current_axis, goalposition, vel, accel);

  return( Pt_CONTINUE );
  
}

