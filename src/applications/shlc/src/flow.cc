/* Y o u r   D e s c r i p t i o n                       */
/*                            AppBuilder Photon Code Lib */
/*                                         Version 1.14C */

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
#include "shlc.h"
#include "abimport.h"
#include "proto.h"


int
disable_opticflow( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	opticflow_mode = false;

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
init_opticflow( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		if (playing_frame < reader->GetSequenceRef().GetNumElements()-1)
		{
			// use current frame as starting point.
			opticflow_mode = true;

			YARPColorConverter::RGB2Grayscale (reader->GetSequenceRef().GetImageRef(playing_frame), *mono);
			velocity_field->Init (*mono);
		}
	}	

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

