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
extern int bright;
extern int hue;
extern int contrast;
extern int satu;
extern int satv;

int
base_open( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	char buff[100];
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	itoa(bright,buff,10);
	PtTextModifyText(ABW_text_bright,0,sizeof(buff),0,buff,sizeof(buff));	
	itoa(hue,buff,10);
	PtTextModifyText(ABW_text_hue,0,sizeof(buff),0,buff,sizeof(buff));	
	itoa(contrast,buff,10);
	PtTextModifyText(ABW_text_contrast,0,sizeof(buff),0,buff,sizeof(buff));	
	itoa(satu,buff,10);
	PtTextModifyText(ABW_text_satu,0,sizeof(buff),0,buff,sizeof(buff));	
	itoa(satv,buff,10);
	PtTextModifyText(ABW_text_satv,0,sizeof(buff),0,buff,sizeof(buff));	

	return( Pt_CONTINUE );
}

