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
extern PhImage_t * phimage_vis;
extern int net_img_w;
extern int net_img_h;
extern int W;
extern int H;
extern double * vv;
extern double * uu;
extern bool m_exit;
extern char _name[512];
//Dibujo is the spanish work for draw. Sorry for the inconvenience but it is not very easy
//to change this name now
void dibujo( PtWidget_t *widget, PhTile_t *damage )  
{
	PhPoint_t pos;

	/* snap the time */
	pos.x = 0;
	pos.y = 0;
	if ( phimage != NULL && !m_exit)   
	{
		if( net_img_w != W || net_img_h != H )
		{
			ulong_t value;
			for (int i = 0; i < H; i++)
				for (int j = 0; j < W; j++)
				{
					PiGetPixel(phimage,(ushort_t)uu[j],(ushort_t)vv[i],&value);
					PiSetPixel(phimage_vis,j,i,value);
				}

			PgDrawPhImagemx(&pos,phimage_vis,0);
			PtTextModifyText(ABW_name_text,0,sizeof(_name),0,_name,sizeof(_name));

		}else
		{
			PgDrawPhImagemx( &pos, phimage, 0);
		}
	}
	//PgDrawPhImagemx(&pos, phimage,0);
	/* Reset the clipping area. */
	PgFlush();
}

