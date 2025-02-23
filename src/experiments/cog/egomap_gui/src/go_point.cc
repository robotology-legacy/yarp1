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
#include "abimport.h"
#include "proto.h"
#include "common.h"

#include <assert.h>

int
go_point( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

	{

	  PhPointerEvent_t *event_data;
	  PhRect_t *rect;
	  PhRect_t raw;
	  short x, y;
	  double lx, uy, rx, ly, fx, fy;
	  
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	  //      printf("Coo-coo!\n");
	  assert(cbinfo!=NULL);
	  assert(cbinfo->event!=NULL);
	  if (cbinfo->event->type == Ph_EV_BUT_PRESS)
	    {
	      //printf("--> Coooo!\n");
	      //assert(cbinfo->cbdata!=NULL);
	      event_data = (PhPointerEvent_t*) PhGetData(cbinfo->event);
	      rect = PhGetRects(cbinfo->event);
	      PtBasicWidgetCanvas(widget,&raw);
	      assert(rect!=NULL);
	      //x = event_data->pos.x;
	      //y = event_data->pos.y;
	      /*
	       *              printf("-->    %d %d (%d %d / %d %d)\n", rect->ul.x, rect->ul.y,
				     raw.ul.x, raw.ul.y,
				     raw.lr.x, raw.lr.y);
	       *               */
	      x = rect->ul.x;
	      y = rect->ul.y;
	      lx = raw.ul.x; uy = raw.ul.y;
	      rx = raw.lr.x; ly = raw.lr.y;
	      fx = (x-lx)/(rx-lx)-0.5;
	      fy = -((y-ly)/(uy-ly)-0.5);
	      SayBottle('d',fx, fy);
	    }
	  
	return( Pt_CONTINUE );

	}

