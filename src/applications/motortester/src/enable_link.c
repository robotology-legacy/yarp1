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

int master_axis, slave_axis;

void set_link(int update_button_state)
{
  PtArg_t args[1];
  double link_ratio;
  
  master_axis = read_selected_item_from_widget(ABW_link_master_selector) - 1;
  slave_axis = read_selected_item_from_widget(ABW_link_slave_selector) - 1;
  link_ratio = read_double_from_widget(ABW_link_ratio);
  
  if ((slave_axis < 0) || (slave_axis > 7))
    {
      sprintf(errmsg, "Tried to link to slave axis %d\n", slave_axis);
      ApError(ABW_base, NULL, "enable_link", errmsg,
	      __FILE__);
      return;
    }

  if ((master_axis < 0) || (master_axis > 7))
    {
      sprintf(errmsg, "Tried to link to master axis %d\n", master_axis);
      ApError(ABW_base, NULL, "enable_link", errmsg,
	      __FILE__);
      return;
    }
  
  if (master_axis == slave_axis)
    {
      sprintf(errmsg, "Tried to link master axis %d to self\n", master_axis);
      ApError(ABW_base, NULL, "enable_link", errmsg,
	      __FILE__);
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, FALSE, 0);
      PtSetResources(ABW_link_enable_button, 1, args);
      return;
    }
  
  disable_widget(ABW_link_master_selector);
  disable_widget(ABW_link_slave_selector);
  disable_widget(ABW_link_ratio);
  
  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, TRUE, 0);
      PtSetResources(ABW_link_enable_button, 1, args);
    }

  // printf("linking... master axis %d to slave axis %d, ratio %f\n",
  // master_axis, slave_axis, link_ratio);
  meid_mei_link(meidx, master_axis, slave_axis,
		link_ratio, LINK_ACTUAL);
}

void remove_link(int update_button_state)
{
  PtArg_t args[1];

  meid_endlink(meidx, slave_axis);

  enable_widget(ABW_link_master_selector);
  enable_widget(ABW_link_slave_selector);
  enable_widget(ABW_link_ratio);
  
  if (update_button_state)
    {
      PtSetArg(&args[0], Pt_ARG_ONOFF_STATE, FALSE, 0);
      PtSetResources(ABW_link_enable_button, 1, args);
    }
  
  // printf("ending link\n");
}

int
enable_link( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtOnOffButtonCallback_t *cback;
  int button_state;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  /* Update the global link state variable */
  cback = cbinfo->cbdata;
  button_state = cback->state;

  if (button_state != 0) {
    set_link(FALSE);
  } else {
    remove_link(FALSE);
  }
  
  return( Pt_CONTINUE );
  
}

