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
#include <YARPMatrix.h>
#include <YARPControlBoardNetworkData.h>

extern PtWidget_t * praw;
extern YARPControlBoardNetworkData _arm_status;

int
RawActivate( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	char buff[100];
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;
	PtDamageWidget( praw );
	return( Pt_CONTINUE );

}

