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

#define FROM_WIDGET ABW_RunningList

int
KillButton_OnClick( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )

{
  unsigned short index;
  PtArg_t args[1]; 
  PtArg_t args2[1]; 
  short *num, *numi, i; 
  char **items = NULL;
  char *str;
  unsigned short *indices = NULL;

  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  PtSetArg(&args[0], Pt_ARG_ITEMS, &items, &num);
  PtGetResources(FROM_WIDGET, 1, args);
  PtSetArg(&args2[0], Pt_ARG_SELECTION_INDEXES, &indices, &numi);
  PtGetResources(FROM_WIDGET, 1, args2); 

  for (i = 0; i < *numi; i++) 
    {
      str = items[indices[i]-1];
      printf("stop %s\n", str);
      fflush(stdout);
    }

  /*
  for (i = 0; i < *numi; i++) 
    {
      index = PtListItemPos(TO_WIDGET,items[indices[i]-1]);
      if (index == 0)
	{
	  PtListAddItems(TO_WIDGET,&items[indices[i]-1],1,0);
	}
      index = PtListItemPos(TO_WIDGET,items[indices[i]-1]);
      if (index != 0)
	{
	  PtListSelectPos(TO_WIDGET,index);
	}
    }

  while (*numi)
    {
      PtListRemovePositions(FROM_WIDGET,&indices[0],1);
      PtSetArg(&args2[0], Pt_ARG_SELECTION_INDEXES, &indices, &numi);
      PtGetResources(FROM_WIDGET, 1, args2); 
    }
  */
  
  return( Pt_CONTINUE );

}

