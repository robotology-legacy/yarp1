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
#include "cogcam.h"
#include "abimport.h"
#include "proto.h"

int image_scale = 1;

void set_size()
{
  PhDim_t *dp, d;
  PtArg_t args[2];
  PhPoint_t pt;
  PhPoint_t *pp;
  int i;
  
  // dim.w = FRAME_WIDTH*image_scale;
  // dim.h = FRAME_HEIGHT*image_scale;
  
  dp = &d;
  pp = &pt;
  
  d.h = FRAME_HEIGHT * image_scale + 112;
  d.w = FRAME_WIDTH * image_scale + 14;
  PtSetArg(&args[0], Pt_ARG_DIM, dp, 0);
  PtSetResources(ABW_base, 1, args);
  d.h=FRAME_HEIGHT * image_scale + 4;
  d.w=FRAME_WIDTH  * image_scale + 4;
  PtSetArg(&args[0], Pt_ARG_DIM, &d, 0);
  pt.x = 4;
  pt.y = 4;
  PtSetArg(&args[1], Pt_ARG_POS, &pt, 0);
  PtSetResources(ABW_raw_label, 2, args);
  pt.x = 4;
  pt.y = FRAME_HEIGHT * image_scale + 10;
  PtSetArg(&args[0], Pt_ARG_POS, &pt, 0);
  PtSetResources(ABW_command_group, 1, args);

  SetupImage(&ptest, FRAME_WIDTH*image_scale, FRAME_HEIGHT*image_scale, 3);
  setup_DB(ptest.size.w, ptest.size.h);
  
  PtMoveResizeWidget(ABW_raw_label, 0);
  
  PtFlush();
}

int
resize_bigger( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  image_scale++;
  
  if (image_scale > MAX_SCALE) image_scale = MAX_SCALE;

  set_size();
  
  return( Pt_CONTINUE );

}


int
resize_smaller( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  image_scale--;
  
  if (image_scale < 1) image_scale = 1;
  
  set_size();
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
  
}


int
initial_size( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  image_scale = 1;

  set_size();

  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  return( Pt_CONTINUE );
  
}

