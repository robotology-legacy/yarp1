/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 2.01  */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

PtWidget_t * praw;
PhRect_t     raw_canvas;
extern int net_img_h;
extern int net_img_w;
extern PhImage_t * phimage;

int raw_init( PtWidget_t *widget )  
{

  //PtSuperClassDraw( PtBasic, widget, damage );
  PtCalcCanvas(widget, &raw_canvas);

  phimage = PhCreateImage( phimage,
			   net_img_w,
			   net_img_h,
			   Pg_IMAGE_DIRECT_888,
			   NULL,
			   0,
			   1);
  phimage->bpl = net_img_w * 3;
  
  praw = widget;
  return PtSuperClassInit( PtBasic, widget );
}

