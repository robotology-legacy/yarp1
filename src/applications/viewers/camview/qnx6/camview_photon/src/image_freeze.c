/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

extern bool freeze;

int
image_freeze( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
  /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  if (!freeze)
  {
    freeze = true;
    ApModifyItemText( &imagemenu, ABN_image_frezee, "DiFreeze");
  }
  else
  {
    freeze = false;
    ApModifyItemText( &imagemenu, ABN_image_frezee, "Freeze");
  }

  return( Pt_CONTINUE );
}

