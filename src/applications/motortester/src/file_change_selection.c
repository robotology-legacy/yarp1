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
file_change_selection( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  char *newfile;
  PtArg_t args[1];

  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  newfile = read_text_from_widget(ABW_selection_input);
  // printf("new file <%s>\n", newfile);

  PtSetArg(&args[0], Pt_ARG_FS_ROOT_DIR, newfile, 0);
  PtSetResources(ABW_file_tree, 1, args);
  
  return( Pt_CONTINUE );
}

