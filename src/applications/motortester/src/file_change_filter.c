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
file_change_filter( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  char *newfilter;
  PtArg_t args[2];
  PtFileSelItem_t *rootitem;
  
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  newfilter = read_text_from_widget(ABW_filter_input);
  // printf("new filter <%s>\n", newfilter);

  rootitem = PtFSRootItem(ABW_file_tree);

  PtSetArg(&args[0], Pt_ARG_FS_FILE_SPEC, newfilter, 0);
  PtSetArg(&args[1], Pt_ARG_FS_REFRESH, rootitem, 0);
  PtSetResources(ABW_file_tree, 2, args);
  
  return( Pt_CONTINUE );
  
}

