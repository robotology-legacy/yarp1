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
#include <math.h>

extern PhImage_t * phimage;
extern PhImage_t * phimage_vis;
extern int W;
extern int H;
extern int net_img_w;
extern int net_img_h;
double * uu;
double * vv;

int
base_resize( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	unsigned short  height;
	unsigned short  width;
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	PtContainerCallback_t * cbdata = (PtContainerCallback_t *)cbinfo->cbdata;

	//PtGetResource( widget, Pt_ARG_HEIGHT, &height, 0 );
	//PtGetResource( widget, Pt_ARG_WIDTH, &width, 0 );
	height = cbdata->new_dim.h;
	width = cbdata->new_dim.w;

	H = (height) - ((H * 0.2 ) + 50);
	W = (width) - 20;

	if (uu != NULL)
		delete []uu;
	uu = new double[W];

	if (vv != NULL)
		delete []vv;
	vv = new double[H];

	double dx = (double)net_img_w / (double)W;
	double dy = (double)net_img_h / (double)H;

	double uu_temp[W];
	double vv_temp[H];

	uu[0] = 0;
	vv[0] = 0;
	uu_temp[0] = 0;
	vv_temp[0] = 0;

	double integer_part;
	double fractional_part;

	for (int i = 1; i < H; i++)
	{
		vv_temp[i] = vv_temp[i-1] + dy;
		fractional_part = modf(vv_temp[i], &integer_part);
		vv[i] = (fractional_part < 0.5) ? integer_part : (integer_part + 1);
	}

	for (int j = 1; j < W; j++)
	{
		uu_temp[j] = uu_temp[j-1] + dx;
		fractional_part = modf(uu_temp[j], &integer_part);
		uu[j]   = (fractional_part < 0.5) ? integer_part : (integer_part + 1);
	}

	if( phimage_vis != NULL )
	{
		phimage_vis->flags = Ph_RELEASE_IMAGE_ALL;
		PhReleaseImage(phimage_vis);
	}
	//Create new image with the new window dimension
	phimage_vis = PhCreateImage( NULL,
			W,
			H,
			Pg_IMAGE_DIRECT_888,
			NULL,
			0,
			1);
	phimage_vis->bpl = W * 3;

	PhPoint_t new_pos;

	new_pos.x = 10;
	new_pos.y = H +5;

	PtSetResource( ABW_name_text, Pt_ARG_POS, &new_pos, 0 );

	return( Pt_CONTINUE );
}

