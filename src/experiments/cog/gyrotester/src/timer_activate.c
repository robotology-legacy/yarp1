/* Called every time the timer activates... updates data from the */
/* ISD daemon and draws to the display */
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
#include "gyrotester.h"
#include "abimport.h"
#include "proto.h"
#include "math.h"
#include "time.h"

#define REDRAW_THRESHOLD 0.0 // cuts down on the flicker

float yaw, pitch, roll;
int iyaw, ipitch, iroll;
float lastyaw = 0.0;
float lastpitch = 0.0;
float lastroll = 0.0;

clock_t last_time = 0;
clock_t this_time = 0;

char yaw_label[20];
char pitch_label[20];
char roll_label[20];
char timestring[20];

int
timer_activate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtArg_t args[1];
  float diffsum;
  clock_t diff_time;
  
  isd_poll_all(isdx, &yaw, &pitch, &roll);
  
  iyaw = floor(yaw);
  ipitch = floor(pitch);
  iroll = floor(roll);
  
  PtSetArg(&args[0], Pt_ARG_GAUGE_VALUE, iyaw, 0);
  PtSetResources(ABW_yaw_meter, 1, args);
  PtSetArg(&args[0], Pt_ARG_GAUGE_VALUE, ipitch, 0);
  PtSetResources(ABW_pitch_meter, 1, args);
  PtSetArg(&args[0], Pt_ARG_GAUGE_VALUE, iroll, 0);
  PtSetResources(ABW_roll_meter, 1, args);
  
  sprintf(yaw_label, "%7.2f", yaw);
  sprintf(pitch_label, "%7.2f", pitch);
  sprintf(roll_label, "%7.2f", roll);
  
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &yaw_label, 1);
  PtSetResources(ABW_yaw_label, 1, args);
  
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &pitch_label, 1);
  PtSetResources(ABW_pitch_label, 1, args);

  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &roll_label, 1);
  PtSetResources(ABW_roll_label, 1, args);
	   
  diffsum = abs(yaw - lastyaw);
  diffsum += abs(pitch - lastpitch);
  diffsum += abs(roll - lastroll);
  
  if (diffsum >= REDRAW_THRESHOLD)
    {
      lastyaw = yaw;
      lastpitch = pitch;
      lastroll = roll;
      PtDamageWidget(ABW_rawcube);
      PtFlush();
    }
  
  last_time = this_time;
  this_time = clock();
  
  diff_time = this_time - last_time;
  sprintf(timestring, "%d Hz", CLOCKS_PER_SEC/diff_time);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, &timestring, 1);
  PtSetResources(ABW_rate_label, 1, args);
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  return( Pt_CONTINUE );
  
}

