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

char pos_label[20];

int
update_position_trend( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  double pos;
  short position_array[1];
  int status, state;
  char *statelabel;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  meid_get_position(meidx, current_axis, &pos);
  status = meid_axis_status(meidx, current_axis);
  state = meid_axis_state(meidx, current_axis);
  
  position_array[0] = pos;
  
  if ((!trigger_enabled) || (trigger_time > 0))
    {
      PtSetArg(&args[0], Rt_ARG_TREND_DATA, position_array, 1);
      PtSetResources(ABW_position_trend, 1, args);
    }
  
  if (trigger_enabled && trigger_fired)
    {
      if (trigger_time > 0)
	trigger_time--;
    }
  
  if (repeat_enabled)
    {
      if (meid_axis_done(meidx, current_axis))
	{
	  switch(repeat_next_goal)
	    {
	    case REPEAT_GOAL_1:
	      meid_start_move(meidx, current_axis, repeat_goal1, 
			      repeat_vel, repeat_accel);
	      repeat_next_goal = REPEAT_GOAL_2;
	      break;
	    case REPEAT_GOAL_2:
	      meid_start_move(meidx, current_axis, repeat_goal2, 
			      repeat_vel, repeat_accel);
	      repeat_next_goal = REPEAT_GOAL_1;
	      break;
	    default:
	      break;
	    }
	}
    }
  if (state < 2)
    { 
      statelabel = "NO_EVENT";
    }
  else
    {
      switch(state)
	{
	case NEW_FRAME:
	  statelabel = "NEW_FRAME";
	  break;
	case STOP_EVENT:
	  statelabel = "STOP_EVENT";
	  break;
	case E_STOP_EVENT:
	  statelabel = "E_STOP_EVENT";
	  break;
	case ABORT_EVENT:
	  statelabel = "ABORT_EVENT";
	  break;
	default:
	  statelabel = "UNKNOWN";
	  break;
	}
    }
  
  sprintf(pos_label, "%d", (int) pos);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &pos_label, 1);
  PtSetResources(ABW_position_label, 1, args);
  
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, status & IN_SEQUENCE, 0);
  PtSetResources(ABW_in_sequence_light, 1, args);
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, status & IN_POSITION, 0);
  PtSetResources(ABW_in_position_light, 1, args);
  PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, status & IN_MOTION, 0);
  PtSetResources(ABW_in_motion_light, 1, args);

  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, statelabel, 0);
  PtSetResources(ABW_axis_state_label, 1, args);

  return( Pt_CONTINUE );
}

