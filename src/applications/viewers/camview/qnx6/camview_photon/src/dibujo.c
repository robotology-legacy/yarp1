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

#include <sys/neutrino.h>
#include <inttypes.h>
#include <sys/syspage.h>

/* Local headers */
#include "ablibs.h"
#include "abimport.h"
#include "proto.h"

int state = 0;
int fd;
extern PhRect_t raw_canvas;
extern PhImage_t * phimage;

//Dibujo is the spanish work for draw. Sorry for the inconvenience but it is not very easy
//to change this name now
void dibujo( PtWidget_t *widget, PhTile_t *damage )  
{
   PhPoint_t pos;

   /* snap the time */
   pos.x = 0;
   pos.y = 0;

   if ( phimage->image != NULL)   
	PgDrawPhImagemx( &pos, phimage, 0);
   /* Reset the clipping area. */
   PgFlush();
}

