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
realized_dir_name( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t args[2];
	int len = strlen(globaldirname);
	PtSetArg(&args[0], Pt_ARG_TEXT_STRING, globaldirname, len);
	PtSetArg(&args[1], Pt_ARG_CURSOR_POSITION, len-1, 0);
	PtSetResources(ABW_files_dir_name, 2, args);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
unrealized_dir_name( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
change_dir_name( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtFileSelItem_t *item;

	item = PtFSGetCurrent(ABW_files_file_selector);

	if (item == NULL)
		return (Pt_CONTINUE);

	if (item->type == Pt_FS_DIR_OP ||
		item->type == Pt_FS_DIR_CL ||
		item->type == Pt_FS_DLINK_OP ||
		item->type == Pt_FS_DLINK_CL)
	{
		//printf ("Shcl: selction is a directory\n");
		//printf ("path: %s\n", item->fullpath);

		sprintf (globaldirname, "%s\0", item->fullpath);
		PtArg_t args[2];
		int len = strlen(globaldirname);
		PtSetArg(&args[0], Pt_ARG_TEXT_STRING, globaldirname, len);
		PtSetArg(&args[1], Pt_ARG_CURSOR_POSITION, len-1, 0);
		PtSetResources(ABW_files_dir_name, 2, args);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
load_data( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (globaldirname[0] != '\0')
	{
		int ret = reader->GetSequenceRef().Load(globaldirname, "img");
		if (ret < 0)
		{
			//
			PtMessageBox (ABW_base, "Error!", "Load failed", "helv14", "&Ok");
		}

		reader->GetOutputSequenceRef().UpToDate() = false;

		ret = reader->GetOutputSequenceRef().Load(globaldirname, "img");
		if (ret >= 0)
		{
			// return
			//return (Pt_CONTINUE);
			reader->GetOutputSequenceRef().UpToDate() = true;
		}

		if (play_from_memory)
		{
			playing_frame = 0;

			render_cur_frame ();
			render_sequence ();

			if (reader->GetOutputSequenceRef().UpToDate())
			{
				render_cur_output_frame ();
				render_output_sequence ();
			}

			// plot position.
			PtDamageWidget (ABW_base_plot1);
			PtDamageWidget (ABW_base_plot2);
		}
	}
	else
	{
		PtMessageBox (ABW_base, "Error!", "Load failed", "helv14", "&Ok");
	}
		
	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
save_data( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (!play_from_memory)
		return (Pt_CONTINUE);

	if (globaldirname[0] != '\0')
	{
		int ret = reader->GetSequenceRef().Save(globaldirname, "img");
		if (ret < 0)
		{
			//
			PtMessageBox (ABW_base, "Error!", "Save failed", "helv14", "&Ok");
			return (Pt_CONTINUE);
		}

		if (!reader->GetOutputSequenceRef().UpToDate())
		{
			printf ("No output to save!\n");
			return (Pt_CONTINUE);
		}

		ret = reader->GetOutputSequenceRef().Save(globaldirname, "img");
		if (ret < 0)
		{
			//
			PtMessageBox (ABW_base, "Error!", "Save output failed", "helv14", "&Ok");
			return (Pt_CONTINUE);
		}
	}
	else
	{
		PtMessageBox (ABW_base, "Error!", "Save failed", "helv14", "&Ok");
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

