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

void set_repeat(int update_button_state)
{
  PtArg_t args[1];
  
  repeat_enabled = TRUE;
  repeat_goal1 = read_integer_from_widget(ABW_goal_position_1);
  repeat_goal2 = read_integer_from_widget(ABW_goal_position_2);
  repeat_vel = read_double_from_widget(ABW_velocity_input);
  repeat_accel = read_double_from_widget(ABW_acceleration_input);
  repeat_next_goal = REPEAT_GOAL_2;
  
  disable_widget(ABW_zero_button);
  disable_widget(ABW_goto_goal_1);
  disable_widget(ABW_goto_goal_2);
  disable_widget(ABW_goal_position_1);
  disable_widget(ABW_goal_position_2);
  disable_widget(ABW_velocity_input);
  disable_widget(ABW_acceleration_input);
  disable_widget(ABW_link_enable_button);
  
  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, trigger_enabled, 0);
      PtSetResources(ABW_repeat_button, 1, args);
    }
  
  // start the first motion
  meid_start_move(meidx, current_axis, repeat_goal1, repeat_vel, repeat_accel);
  
  // printf("Goal1=%d  Goal2=%d vel=%f accel=%f\n",
  // repeat_goal1, repeat_goal2, repeat_vel, repeat_accel);
}

void remove_repeat(int update_button_state)
{
  PtArg_t args[1];
  
  repeat_enabled = FALSE;
  repeat_goal1 = 0;
  repeat_goal2 = 0;
  repeat_vel =   0.0;
  repeat_accel = 0.0;
  repeat_next_goal = REPEAT_GOAL_NONE;
  
  enable_widget(ABW_zero_button);
  enable_widget(ABW_goto_goal_1);
  enable_widget(ABW_goto_goal_2);
  enable_widget(ABW_goal_position_1);
  enable_widget(ABW_goal_position_2);
  enable_widget(ABW_velocity_input);
  enable_widget(ABW_acceleration_input);
  enable_widget(ABW_link_enable_button);

  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, trigger_enabled, 0);
      PtSetResources(ABW_repeat_button, 1, args);
    }
}

int
enable_repeat( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtOnOffButtonCallback_t *cback;
  int button_state;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  /* Update the global link state variable */
  cback = cbinfo->cbdata;
  button_state = cback->state;

  if (button_state != 0) {
    set_repeat(FALSE);
  } else {
    remove_repeat(FALSE);
  }
  
  return( Pt_CONTINUE );
  
}

