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


int
file_set_selection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  PtFileSelItem_t *item; 
  char *selection_name;
  PtArg_t args[2];
  int len;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;
  
  item = PtFSGetCurrent(ABW_file_tree); 
  if (item == NULL) 
    selection_name = "";
  else
    selection_name = item->fullpath;

  // printf("selection name <%s>\n", selection_name);
  len = strlen(selection_name);
  PtSetArg(&args[0], Pt_ARG_TEXT_STRING, selection_name, len);
  PtSetArg(&args[1], Pt_ARG_CURSOR_POSITION, len-1, 0);
  PtSetResources(ABW_selection_input, 2, args);

  return( Pt_CONTINUE );
}

