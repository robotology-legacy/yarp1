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


int
MakeButton_OnClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  unsigned short index;
  PtArg_t args[1]; 
  PtArg_t args2[1]; 
  short *num, *numi, i; 
  char **items = NULL;
  unsigned short *indices = NULL;

  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

   PtSetArg(&args[0], Pt_ARG_ITEMS, &items, &num);
  PtGetResources(ABW_ActiveList, 1, args);
  PtSetArg(&args2[0], Pt_ARG_SELECTION_INDEXES, &indices, &numi);
  PtGetResources(ABW_ActiveList, 1, args2); 
  
  for (i = 0; i < *numi; i++) 
    {
      printf("start %s\n", items[indices[i]-1]);
      fflush(stdout);
    }
 

  return( Pt_CONTINUE );  
}

