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
#include "motortester.h"
#include "abimport.h"
#include "proto.h"

#define MAX_AXES PCDSP_MAX_AXES

int
soft_shutdown( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  int i, psum;
  int16 allgains[MAX_AXES][COEFFICIENTS];
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  psum = 0;
  for(i=0;i<MAX_AXES;i++)
    {
      meid_get_filter(meidx, i, &allgains[i]);
      allgains[i][DF_I] = 0;
      psum += allgains[i][DF_P];
    }
  
  while(psum > 0)
    {
      psum = 0;
      for(i=0;i<MAX_AXES;i++)
	{
	  if (allgains[i][DF_P] > 0)
	    {
	      allgains[i][DF_P] = allgains[i][DF_P] * .98;
	      meid_set_filter(meidx, i, &allgains[i]);
	    }
	  psum += allgains[i][DF_P];
	}
      draw_gains();
      PtFlush();
    }

  for(i=0;i<MAX_AXES;i++)
    {
      meid_get_filter(meidx, i, &allgains[i]);
      allgains[i][DF_D] = 0;
      meid_set_filter(meidx, i, &allgains[i]);
    }
  
  draw_gains();
  PtFlush();
   
  return( Pt_CONTINUE );
  
}

