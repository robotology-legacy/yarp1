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
#include "ReadingThread.h"

extern ReadingThread t1;
extern bool freeze;
extern double * vv;
extern double * uu;

int
exit_viewer( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
      /* eliminate 'unreferenced' warnings */
  widget = widget, apinfo = apinfo, cbinfo = cbinfo;

  freeze = true;
  m_exit = true;
  if (vv!=NULL)
    delete []vv;
  if (uu!=NULL)
    delete []uu;
  t1.End(0);

  PtExit( EXIT_SUCCESS );

  return( Pt_CONTINUE );
}

