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

#include "YARPFlowTracker.h"

extern YARPFlowTracker *tracker;

int
enable_recording( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (!play_from_memory)
	{
		reader->ToggleRecording();
		printf ("Shlc: recording enabled flag: %d\n", reader->IsRecordingEnabled());
	}
	else
	{
		//
		PtArg_t arg[1];
		PtSetArg (&arg[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
		PtSetResources (widget, 1, &arg[0]);
		printf ("Shlc: cannot enable recording - playing from memory\n");
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
toggle_freeze_display( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	manager->ToggleFreeze();

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
connect( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int ret = spawnlp (P_WAIT, CONNECT_SCRIPT_NAME, CONNECT_SCRIPT_NAME, NULL);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}

int 
render_cur_frame ()
{
	// display cur frame.
	YARPImageOf<YarpPixelBGR>& tmp = reader->GetSequenceRef().GetImageRef(playing_frame);
	oos->Display (ABW_base_input_img, tmp);

	// update GUI.
	sprintf (genericbuf, "Frame = %4d of %4d\0", playing_frame, reader->GetSequenceRef().GetNumElements());	
	PtArg_t arg[1];
	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_player_frame_no, 1, &arg[0]);		

	return 0;
}

int 
render_cur_output_frame ()
{
	// display cur frame.
	YARPImageOf<YarpPixelBGR>& tmp = reader->GetOutputSequenceRef().GetImageRef(playing_frame);
	oosd->Display (ABW_base_output_img, tmp);

	// update GUI (THIS MUST BE THE SAME OF RENDER_CUR_FRAME).
	sprintf (genericbuf, "Frame = %4d of %4d\0", playing_frame, reader->GetOutputSequenceRef().GetNumElements());	
	PtArg_t arg[1];
	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_player_frame_no, 1, &arg[0]);		

	return 0;
}

static bool was_enabled = false;

int
toggle_play_from_mem( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	PtArg_t arg[1];
    PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
    PtGetResources (ABW_player_play_from_mem, 1, &arg[0]);

	play_from_memory = ((arg[0].value & Pt_SET) != 0) ? true : false;
	printf ("play_from_memory: %d\n", play_from_memory);

	// from false to true...
	if (play_from_memory)
	{
		// stop recording (if active).
		if (reader->IsRecording() ||
			reader->GetSequenceRef().GetNumElements() <= 0)
		{
			printf ("Shlc: memory store is empty, nothing to display\n");

			// set state to unpressed, return.
			PtSetArg (&arg[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
			PtSetResources (ABW_player_play_from_mem, 1, &arg[0]);

			play_from_memory = false;

			return (Pt_CONTINUE);
		}

		if (reader->IsRecordingEnabled())
		{
			// disable recording while playing.
			PtSetArg (&arg[0], Pt_ARG_FLAGS, Pt_FALSE, Pt_SET);
			PtSetResources (ABW_base_enable_recording, 1, &arg[0]);

			reader->DisableRecording();
			// store value to enable.
			was_enabled = true;
		}

		playing_frame = 0;
		opticflow_mode = false;

		if (canonical_mode == true)
		{
			reset_canonical_mode ();
		}

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
	else	// from true to false.
	{
		// enable recording after playing.
		if (was_enabled)
		{
			reader->EnableRecording();

			PtSetArg (&arg[0], Pt_ARG_FLAGS, Pt_TRUE, Pt_SET);
			PtSetResources (ABW_base_enable_recording, 1, &arg[0]);
		}
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
rewind_memory( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		playing_frame = 0;
		render_cur_frame ();
		opticflow_mode = false;

		if (canonical_mode == true)
		{
			reset_canonical_mode ();
		}

		if (reader->GetOutputSequenceRef().UpToDate())
		{
			render_cur_output_frame ();
		}

		PtDamageWidget (ABW_base_plot1);
		PtDamageWidget (ABW_base_plot2);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
back_memory( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		if (playing_frame > 0)
			playing_frame --;

		opticflow_mode = false;
		if (canonical_mode == true)
		{
			reset_canonical_mode ();
		}

		render_cur_frame ();

		if (reader->GetOutputSequenceRef().UpToDate())
		{
			render_cur_output_frame ();
		}

		PtDamageWidget (ABW_base_plot1);
		PtDamageWidget (ABW_base_plot2);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int render_opticflow (void)
{
	// compute flow.
	YARPColorConverter::RGB2Grayscale (reader->GetSequenceRef().GetImageRef(playing_frame), *mono);
	velocity_field->Apply (*mono);

	*output = reader->GetSequenceRef().GetImageRef(playing_frame);
	velocity_field->FancyDisplay (*output);

	oosd->Display (ABW_base_output_img, *output);

	return 0;
}


int
forward_memory( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		if (playing_frame < reader->GetSequenceRef().GetNumElements()-1)
			playing_frame ++;

		render_cur_frame ();

		if (reader->GetOutputSequenceRef().UpToDate() && !opticflow_mode)
		{
			render_cur_output_frame ();
		}

		PtDamageWidget (ABW_base_plot1);
		PtDamageWidget (ABW_base_plot2);

		if (opticflow_mode)
			render_opticflow ();

		if (canonical_mode == true)
		{
			reset_canonical_mode ();
		}
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
forward_memory_10( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		if (playing_frame < reader->GetSequenceRef().GetNumElements()-10)
			playing_frame += 10;

		opticflow_mode = false;

		if (canonical_mode == true)
		{
			reset_canonical_mode ();
		}

		render_cur_frame ();

		if (reader->GetOutputSequenceRef().UpToDate())
		{
			render_cur_output_frame ();
		}

		PtDamageWidget (ABW_base_plot1);
		PtDamageWidget (ABW_base_plot2);
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int 
render_sequence ()
{
	int numframes = reader->GetSequenceRef().GetNumElements();
	printf ("numframes: %d\n", numframes);

	oosi[0]->Display (ABW_base_i1, reader->GetSequenceRef().GetImageRef(0));
	oosi[1]->Display (ABW_base_i2, reader->GetSequenceRef().GetImageRef(numframes/5));
	oosi[2]->Display (ABW_base_i3, reader->GetSequenceRef().GetImageRef(2*numframes/5));
	oosi[3]->Display (ABW_base_i4, reader->GetSequenceRef().GetImageRef(3*numframes/5));
	oosi[4]->Display (ABW_base_i5, reader->GetSequenceRef().GetImageRef(4*numframes/5));
	oosi[5]->Display (ABW_base_i6, reader->GetSequenceRef().GetImageRef(numframes-1));

	return 0;
}


int 
render_output_sequence ()
{
	int numframes = reader->GetOutputSequenceRef().GetNumElements();
	printf ("numframes: %d\n", numframes);

	ooso[0]->Display (ABW_base_o1, reader->GetOutputSequenceRef().GetImageRef(0));
	ooso[1]->Display (ABW_base_o2, reader->GetOutputSequenceRef().GetImageRef(numframes/5));
	ooso[2]->Display (ABW_base_o3, reader->GetOutputSequenceRef().GetImageRef(2*numframes/5));
	ooso[3]->Display (ABW_base_o4, reader->GetOutputSequenceRef().GetImageRef(3*numframes/5));
	ooso[4]->Display (ABW_base_o5, reader->GetOutputSequenceRef().GetImageRef(4*numframes/5));
	ooso[5]->Display (ABW_base_o6, reader->GetOutputSequenceRef().GetImageRef(numframes-1));

	return 0;
}


int
display_sequence( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (!play_from_memory)
		return( Pt_CONTINUE );

	render_sequence ();
	if (reader->GetOutputSequenceRef().UpToDate())
	{
		render_output_sequence ();
	}

	PtDamageWidget (ABW_base_plot1);
	PtDamageWidget (ABW_base_plot2);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


void draw_plot1( PtWidget_t *widget, PhTile_t *damage )  
{
	if (canonical_mode == true)
	{
		plot_vectors->Draw (widget, damage);
		return;
	}

	if (play_from_memory)
		plot_vectors->SetCurFrame (playing_frame);
	else
		plot_vectors->SetCurFrame (0);

	plot_vectors->SetNumElements (reader->GetSequenceRef().GetNumElements());
	plot_vectors->Draw (widget, damage);
}


void draw_plot2( PtWidget_t *widget, PhTile_t *damage )  
{
	if (canonical_mode == true)
	{
		plot_vectors2->Draw (widget, damage);
		return;
	}

	if (play_from_memory)
		plot_vectors2->SetCurFrame (playing_frame);
	else
		plot_vectors2->SetCurFrame (0);

	plot_vectors2->SetNumElements (reader->GetSequenceRef().GetNumElements());
	plot_vectors2->Draw (widget, damage);
}


int
process_sequence( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	if (play_from_memory)
	{
		// do the processing of the seq in mem.
		reader->ProcessWholeSequence(tracker->GetVisualContactInstance());

		// check whether is fine, uptodate.
		if (reader->GetOutputSequenceRef().UpToDate())
		{
			sprintf (genericbuf, "Output is up to date\0");	
			PtArg_t arg[1];
			PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
			PtSetResources (ABW_base_uptodate, 1, &arg[0]);		
		}
		else
		{
			sprintf (genericbuf, "Output is out of date\0");	
			PtArg_t arg[1];
			PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
			PtSetResources (ABW_base_uptodate, 1, &arg[0]);		
		}

		// more to display.
		// display current out frame and the six frames (LATER).
		render_cur_output_frame ();
		render_output_sequence ();
	}

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
find_object( PtWidget_t *widget, ApInfo_t *apinfo, PtCallbackInfo_t *cbinfo )
{
	int bestaction;
	find_object (bestaction);

	/* eliminate 'unreferenced' warnings */
	widget = widget, apinfo = apinfo, cbinfo = cbinfo;

	return( Pt_CONTINUE );
}


int
reset_canonical_mode (void)
{
	plot_vectors->SetDataPtr (reader->GetSequenceRef().GetArmPositionPtr());
	plot_vectors2->SetDataPtr (reader->GetSequenceRef().GetHeadPositionPtr());
	canonical_mode = false;

	return 0;
}


