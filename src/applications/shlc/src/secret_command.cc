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

//#include "secret.h"

//
// LATER: this is not up to date compared to the LIVE version (see classes.cc).
int
enable_secret( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	secret_mode = true;
	PtArg_t arg[1];

	if (!play_from_memory)
	{
		// set the toggle.
		PtSetArg (&arg[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
		PtSetResources (ABW_player_play_from_mem, 1, &arg[0]);

		// go also into play from mem mode.
		toggle_play_from_mem(widget, apinfo, cbinfo);
	}


	int ret = processing();	
	if (ret >= 0)
	{
		addcanonical ();
		//browser_init();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
disable_secret( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	secret_mode = false;

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
browser_init_cb( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	BrowserInit ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
browser_next_cb( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	BrowserNext ();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int
save_canonical( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	objects->GetCanonical()->Save (CANONICALFILENAME);
	goodaction->Save (GOODMAPFILENAME);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
load_canonical( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	objects->GetCanonical()->Load (CANONICALFILENAME);
	goodaction->Load (GOODMAPFILENAME);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

