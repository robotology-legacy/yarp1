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

int current_axis = 0;
int16 gains[COEFFICIENTS];
int meidx = -1;
char errmsg[100];

int trigger_enabled = FALSE;
int trigger_time = 0;
int trigger_fired = FALSE;

int repeat_enabled = FALSE;
int repeat_next_goal = REPEAT_GOAL_NONE;
int repeat_goal1 = 0;
int repeat_goal2 = 0;
double repeat_vel   = 0.0;
double repeat_accel = 0.0;

char DEVICENAME[256];

#define ACTIVE_COLOR Pg_BLACK
#define INACTIVE_COLOR Pg_MGREY

void disable_widget(PtWidget_t *widget_name)
{
  PtArg_t args[2];
  long flagval;
  PtWidgetClassRef_t *widget_type;
  
  /* change the color of the button text */
  widget_type = PtWidgetClass(widget_name);
  if ((widget_type == PtNumericFloat) ||
      (widget_type == PtNumericInteger))
    {
      PtSetArg(&args[0], Pt_ARG_NUMERIC_TEXT_COLOR, INACTIVE_COLOR, 0);
    }
  else
    {
      PtSetArg(&args[0], Pt_ARG_COLOR, INACTIVE_COLOR, 0);
    }
  /* BLOCK all events to the button */
  flagval = Pt_BLOCKED;
  PtSetArg(&args[1], Pt_ARG_FLAGS, flagval, Pt_BLOCKED);
  PtSetResources(widget_name, 2, args);
}

void enable_widget(PtWidget_t *widget_name)
{
  PtArg_t args[2];
  long flagval;
  PtWidgetClassRef_t *widget_type;
  
  /* change the color of the button text */
  widget_type = PtWidgetClass(widget_name);
  if ((widget_type == PtNumericFloat) ||
      (widget_type == PtNumericInteger))
    {
      PtSetArg(&args[0], Pt_ARG_NUMERIC_TEXT_COLOR, ACTIVE_COLOR, 0);
    }
  else
    {
      PtSetArg(&args[0], Pt_ARG_COLOR, ACTIVE_COLOR, 0);
    }
  
  /* UNBLOCK all events to the button */
  flagval = 0;
  PtSetArg(&args[1], Pt_ARG_FLAGS, flagval, Pt_BLOCKED);
  PtSetResources(widget_name, 2, args);
}

int read_integer_from_widget(PtWidget_t *widget)
{
  PtArg_t args[1];
  int *intval;
  
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &intval, 0);
  PtGetResources(widget, 1, args);
  return(*intval);
}

double read_double_from_widget(PtWidget_t *widget)
{
  PtArg_t args[1];
  double *dval;
    
  PtSetArg(&args[0], Pt_ARG_NUMERIC_VALUE, &dval, 0);
  PtGetResources(widget, 1, args);
  return(*dval);
}

char *read_text_from_widget(PtWidget_t *widget)
{
  PtArg_t args[1];
  char *str;

  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &str, 0);
  PtGetResources(widget, 1, args);
  return(str);
}

int read_selected_item_from_widget(PtWidget_t *widget)
{
  PtArg_t args[1];
  short int *index;
    
  PtSetArg(&args[0], Pt_ARG_CBOX_SEL_ITEM, &index, 0);
  PtGetResources(widget, 1, args);
  return(*index);
}

int
initialize( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int stat;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  /*stat = meid_dsp_init("/yarp/mei/head", &meidx);*/
  meidx = meid_open (DEVICENAME);

  if (meidx == -1)
    {
      printf ("Failed to initialize MEI driver: %s\n", DEVICENAME);
      sprintf(errmsg, "Failed to locate MEI daemon... %s!\n", DEVICENAME);
      ApError(ABW_base, NULL, "initialize", errmsg,
	      __FILE__);
      exit(-1);
    }
  stat = meid_dsp_reset(meidx);
  if (stat != 0)
    {
      sprintf(errmsg, "MEI Reset Failed... Error code %d!\n", stat);
      ApError(ABW_base, NULL, "initialize", errmsg,
	      __FILE__);
      exit(-1);
    }
  
  draw_gains();
  return( Pt_CONTINUE );
}

