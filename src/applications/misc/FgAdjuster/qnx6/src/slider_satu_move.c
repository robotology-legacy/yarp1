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
extern int satu;

int
slider_satu_move( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	char buff[100];
	PtSliderCallback_t * pscb = NULL;
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	pscb = (PtSliderCallback_t *)cbinfo->cbdata;
	satu = pscb->position;
	itoa(satu,buff,10);
	PtTextModifyText(ABW_text_satu,0,sizeof(buff),0,buff,sizeof(buff));	
	sendfginfo();

	return( Pt_CONTINUE );
}

