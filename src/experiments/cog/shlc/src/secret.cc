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
//
//
// GLOBALS.
//
//
//
//

YARPGoodActionMapArray *goodaction = NULL;
YARPObjectContainer *objects = NULL;
YARPFlowTracker *tracker = NULL;

//
//
//
//
int Display (YARPFlowTracker& tracker)
{
	YARPOutputImageSequence& oseq = tracker.GetOutputSeqRef();
	int frame = tracker.GetDisplayFrame();
	int step = tracker.GetDisplayStep();	
	
	CVisDVector v1(3);
	CVisDVector v2(3);
	CVisDVector i1(2);
	CVisDVector i2(2);

	tracker.GetDisplacement (v1, v2, i1, i2);

	const int com_x = int(i1(1)+.5);
	const int com_y = int(i1(2)+.5);
	const int final_x = int(i2(1)+.5);
	const int final_y = int(i2(2)+.5);

	YarpPixelBGR red;
	red.b = 255;
	red.g = red.r = 0;

	AddCircleOutline (oseq.GetImageRef(frame+5*step), red, com_x, com_y, 5);
	AddCircle (oseq.GetImageRef(frame+5*step), red, final_x, final_y, 5);

	oosi[0]->Display (ABW_base_i1, oseq.GetImageRef(frame));
	frame+=step;
	oosi[1]->Display (ABW_base_i2, oseq.GetImageRef(frame));
	frame+=step;
	oosi[2]->Display (ABW_base_i3, oseq.GetImageRef(frame));
	frame+=step;
	oosi[3]->Display (ABW_base_i4, oseq.GetImageRef(frame));
	frame+=step;
	oosi[4]->Display (ABW_base_i5, oseq.GetImageRef(frame));
	
	// should be the contact frame.
	frame+=step;
	oosi[5]->Display (ABW_base_i6, oseq.GetImageRef(frame));
	
	// display the frames after the contact.
	frame+=step;
	ooso[0]->Display (ABW_base_o1, oseq.GetImageRef(frame));
	frame+=step;
	ooso[1]->Display (ABW_base_o2, oseq.GetImageRef(frame));
	frame+=step;
	ooso[2]->Display (ABW_base_o3, oseq.GetImageRef(frame));
	frame+=step;
	ooso[3]->Display (ABW_base_o4, oseq.GetImageRef(frame));
	frame+=step;
	ooso[4]->Display (ABW_base_o5, oseq.GetImageRef(frame));
	frame+=step;
	ooso[5]->Display (ABW_base_o6, oseq.GetImageRef(frame));


	// get the flag and save images.
	PtArg_t arg[1];
    PtSetArg (&arg[0], Pt_ARG_FLAGS, 0, 0);
    PtGetResources (ABW_base_save_frames_toggle, 1, &arg[0]);

	bool save = ((arg[0].value & Pt_SET) != 0) ? true : false;
	if (save)
	{
		int frame = tracker.GetDisplayFrame();
		int step = tracker.GetDisplayStep();	
		char buffer[512];

		for (int i = 0; i < 12; i++)
		{
			sprintf (buffer, "%s/img%04d.ppm\0", CIMAGESFILENAME, i);
			YARPImageFile::Write (buffer, oseq.GetImageRef(frame));
			frame += step;
		}
	}

	return 0;
}

//
//
//
int neuron_index = 0; 
int trace_index = 0;
bool browser_initialized = false;


int BrowserInit (void)
{
	YARPCanonicalNeurons *canonical = objects->GetCanonical();
	assert (canonical != NULL);

	if (canonical->Empty())
	{
		printf ("BrowserInit: nothing to display\n");
		neuron_index = 0;
		trace_index = 0;
		return -1;
	}

	// reset pointers.
	trace_index = 0;
	neuron_index = canonical->Begin();
	(*canonical)[neuron_index].InitCursor();

	browser_initialized = true;

	// display.
	BrowserDisplay ();

	return 0;
}

int BrowserNext (void)
{
	YARPCanonicalNeurons *canonical = objects->GetCanonical();
	assert (canonical != NULL);

	if (canonical->Empty())
	{
		printf ("BrowserNext: nothing to display\n");
		neuron_index = 0;
		trace_index = 0;
		return -1;
	}
	
	if (!browser_initialized)
	{
		printf ("BrowserNext: init browser first\n");
		neuron_index = 0;
		trace_index = 0;
		return -1;
	}

	YARPCanonicalNeuron& n = (*canonical)[neuron_index];
	if (n.EndCursor())
	{
		n.InitCursor();
		neuron_index++;
		trace_index = 0;

		if (neuron_index > canonical->End())
		{
			BrowserInit();
			printf ("BrowserNext: at the end of list, reset\n");
			return -1;
		}

		(*canonical)[neuron_index].InitCursor();
		printf ("browser: new unit displayed\n");
	}
	
	// use (*canonical)[neuron_index].Get();
	BrowserDisplay ();
	
	// increment cursor.
	(*canonical)[neuron_index].IncrementCursor();
	trace_index ++;

	return 0;
}

int BrowserDisplay (void)
{
	YARPCanonicalNeurons *canonical = objects->GetCanonical();
	assert (canonical != NULL);

	// use two windows (e.g. top-left of the GUI) to display the results.
	YARPCanonicalData& d = (*canonical)[neuron_index].Get();
	YARPImageOf<YarpPixelBGR> itmp;
	itmp.Resize (d.m_frame.GetWidth(), d.m_frame.GetHeight());
	itmp = d.m_frame;

	YarpPixelBGR blue;
	blue.r = blue.g = 0;
	blue.b = 255;

	int x = int (d.m_starting_position_image(1) + .5);
	int y = int (d.m_starting_position_image(2) + .5);
	AddCircleOutline (itmp, blue, x, y, 5);

	int xx = int (d.m_ending_position_image(1) + .5);
	int yy = int (d.m_ending_position_image(2) + .5);
	AddCircle (itmp, blue, xx, yy, 5);

	// REMEMBER: I inverted vx!!!!

	int vx = int (d.m_pushing_direction(1)*10 + .5);
	int vy = int (d.m_pushing_direction(2)*10 + .5);
	AddSegment (itmp, blue, x, y, x-vx, y+vy);

	int x1 = x + cos(d.m_object_orientation) * 40;
	int y1 = y + sin(d.m_object_orientation) * 40;
	int x2 = x - cos(d.m_object_orientation) * 40;
	int y2 = y - sin(d.m_object_orientation) * 40;
	AddSegment (itmp, blue, x1, y1, x2, y2);
	AddCircleOutline (itmp, blue, x, y, int(d.m_object_orientation_quality * 100 +.5));

	oos->Display (ABW_base_input_img, itmp);

	YARPImageOf<YarpPixelBGR> tmp;
	tmp.Resize (d.m_mask.GetWidth(), d.m_mask.GetHeight());
	tmp.CastCopy (d.m_mask);

	oosd->Display (ABW_base_output_img, tmp);

	// display also additional info.
	// - action number.
	// - arm, head position.
	// - pushing direction.
	// - contact frame no.
	// - displacement in egocentric coordinates.
	// - displacement in the image plane.

	PtArg_t arg[1];

	sprintf (genericbuf, "Action: %2d Yaw: %.1lf\0", d.m_action_number, d.m_arm_trajectory[d.m_contact_frame](3));	
	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_base_action_no, 1, &arg[0]);		

	//
	// Compute delta in egomap coordinates.
	//
	sprintf (genericbuf, "Delta: %.2f %.2f %.2f\0", (d.m_ending_position(1)-d.m_starting_position(1)), (d.m_ending_position(2)-d.m_starting_position(2)), (d.m_ending_position(3)-d.m_starting_position(3)));	

	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_base_displacement, 1, &arg[0]);		

	sprintf (genericbuf, "Contact: %d \0", d.m_contact_frame);	
	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_base_contact_frame, 1, &arg[0]);		

	sprintf (genericbuf, "Unit: %d t: %d\0", neuron_index, trace_index);	
	PtSetArg (&arg[0], Pt_ARG_TEXT_STRING, genericbuf, 0);
	PtSetResources (ABW_base_unit_no, 1, &arg[0]);		

	canonical_mode = true;

	// unelegant recycling of the display (which also complicate things... modes and the like).
	plot_vectors->SetDataPtr (d.m_arm_trajectory);
	plot_vectors->SetNumElements (d.m_num_steps);
	plot_vectors->SetCurFrame (d.m_contact_frame);

	plot_vectors2->SetDataPtr (d.m_head_trajectory);
	plot_vectors2->SetNumElements (d.m_num_steps);
	plot_vectors2->SetCurFrame (d.m_contact_frame);

	PtDamageWidget (ABW_base_plot1);
	PtDamageWidget (ABW_base_plot2);

	return 0;
}

//
//
//
//
//
//

int 
processing (void)
{
	int ret = tracker->Apply (reader->GetSegmentedImagePort());

	YARPBottle bot;
	YARPBottleIterator it(bot);
	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_PROCESSING_RESULT);
	it.WriteFloat(float(ret));
	SendBottle(bot);

	switch (ret)
	{
		case 0:
		default:
			Display (*tracker); 
			return 0;

		case -1:
			printf ("processing returned without finding a contact and/or optic flow\n");
			return -1;

		case -2:
			printf ("no flow - display only\n");
			Display (*tracker);
			return -2;
	}

	return -3;
}

//
//
// gets data from the 'tracker' and stores it into the 'objects'.
int 
store_last_object_data (void)
{
	YARPBottle bot;
	YARPBottleIterator it(bot);

	YARPImageOf<YarpPixelMono> mask;
	mask.Resize (WIDTH, HEIGHT);
	YARPImageOf<YarpPixelBGR> frame;
	frame.Resize (WIDTH, HEIGHT);
	YARPImageOf<YarpPixelBGR> out;
	out.Resize (WIDTH, HEIGHT);

	CVisDVector v1(3);
	CVisDVector v2(3);
	CVisDVector i1(2);
	CVisDVector i2(2);
	CVisDVector v(2);
	v1 = 0;
	v2 = 0;
	i1 = 0;
	i2 = 0;
	v = 0;

	// collect data.
	tracker->GetObjectProperties (mask, frame);
	tracker->GetDisplacement (v1, v2, i1, i2);

	printf ("starting pos (x,y,z): %lf %lf %lf\n", v1(1), v1(2), v1(3));
	printf ("ending pos (x,y,z): %lf %lf %lf\n", v2(1), v2(2), v2(3));


	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_IMAGE_POSITIONS);
	it.WriteFloat(float(i1(1)));
	it.WriteFloat(float(i1(2)));
	it.WriteFloat(float(i2(1)));
	it.WriteFloat(float(i2(2)));
	SendBottle(bot);


	double orientation = 0;
	double pointiness = 0;
	tracker->GetOrientation (orientation, pointiness);

	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_IMAGE_POSE_SHAPE);
	it.WriteFloat(float(orientation));
	it.WriteFloat(float(pointiness));
	SendBottle(bot);

	int bestaction = -1;

	// find the object in the current frame.
	objects->FindObject (frame);

	// display.
	oosd->Display (ABW_base_output_img, out);
	PtDamageWidget (ABW_base_output_img);

	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_OBJECT_RECOGNIZED_AS);
	it.WriteFloat(float(objects->GetLastKnownObject()));
	SendBottle(bot);

	// these numbers are stored within YARPObjectContainer.
	// to be reused later.
	if (objects->GetLastKnownObject() != -1)
	{
		v = i2 - i1;
		double direction = atan2(v(2), v(1));
		objects->SetLastKnownObjectPose (orientation, atan2(v(2), v(1)));

		
		// OBJECT RECOGNIZED. Interpret the action also.
		// This is not useful for behavior but it gives info about the action (for the future).
		int onumber = objects->GetLastKnownObject();

		double diff = direction - orientation;
		diff = fabs (fmod (diff, pi));
		if (diff > pi/2) diff = pi - diff;

		printf ("------- the error in the observed action was %lf\n", diff * radToDeg);
		printf ("------- obj orient %lf displ %lf\n", orientation * radToDeg, direction * radToDeg);

		// find out about the behavior of the object.
		const double affordance = objects->GetCanonicalStats()->GetPrincipalAffordance (onumber);
		
		if (fabs (affordance - diff) > pi/4)
		{
			printf ("***** -> the object did NOT satisfy its principal affordance\n");
		}

		// search for the best affordance.
		double o1 = direction;
		printf ("---- object: %d, affordance: %lf, test1: %lf\n", onumber, affordance*radToDeg, o1*radToDeg);

		if (o1 > pi/2) o1 -= pi;
		else
		if (o1 < -pi/2) o1 += pi;
		
		double score1 = 0;
		int bestaction1 = objects->GetCanonicalStats()->GetBestActionGeneric (o1, score1);

		printf ("---- score %lf\n", score1);
		printf ("***** PERCEIVED ACTION ***** %d\n", bestaction1);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_ACTION_INTERPRETED_AS);
		it.WriteFloat(float(bestaction1));
		SendBottle(bot);

		//
		//
		//
	}
	else
	{
		printf ("*** object is not known, or not found\n");
		objects->ResetObject();
		return -1;
	}

	return 0;
}


//
//
// complicated! gets data from various places and adds to the canonical representation
//	contained within objects.
int 
addcanonical (void)
{
	bool notadd = objects->IsActive();

	YARPImageOf<YarpPixelMono> mask;
	mask.Resize (WIDTH, HEIGHT);
	YARPImageOf<YarpPixelBGR> frame;
	frame.Resize (WIDTH, HEIGHT);
	YARPImageOf<YarpPixelFloat> tmp;
	tmp.Resize (WIDTH, HEIGHT);

	YARPBottle bot;
	YARPBottleIterator it(bot);

	CVisDVector v1(3);
	CVisDVector v2(3);
	CVisDVector i1(2);
	CVisDVector i2(2);
	v1 = 0;
	v2 = 0;
	i1 = 0;
	i2 = 0;

	// collect all the data.
	tracker->GetObjectProperties (mask, frame);
	tracker->GetDisplacement (v1, v2, i1, i2);

	printf ("starting pos (x,y,z): %lf %lf %lf\n", v1(1), v1(2), v1(3));
	printf ("ending pos (x,y,z): %lf %lf %lf\n", v2(1), v2(2), v2(3));

	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_IMAGE_POSITIONS);
	it.WriteFloat(float(i1(1)));
	it.WriteFloat(float(i1(2)));
	it.WriteFloat(float(i2(1)));
	it.WriteFloat(float(i2(2)));
	SendBottle(bot);


	CVisDVector pushing(2);
	tracker->GetPushingDirection (pushing);


	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_PUSHING_VECTOR);
	it.WriteFloat(float(pushing(1)));
	it.WriteFloat(float(pushing(2)));
	SendBottle(bot);


	// from the reader, query the sequence for motor data.
	// get the pointers to Sequence (copy only once later).
	int start = 0;
	int stop = 0;
	CVisDVector *phead;
	CVisDVector *parm;
	int contact = 0;
	tracker->GetMotorData (start, stop, &phead, &parm, contact);	

	int action;
	tracker->GetActionID (action);


	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_ISSUED_ACTION);
	it.WriteFloat(float(action));
	SendBottle(bot);


	double reinforcer;
	tracker->GetReinforcer (reinforcer);

	// build histogram and compare with existing object.
	YARPVisualSearch vs;	
	vs.Reset ();
	tmp.CastCopy (mask);
	vs.Add (frame, tmp);

	// if new (scan the existing objects).
	YARPVisualSearch vss;
	vss.Reset ();

	float best_match = 0;
	int best_match_neuron = -1;
	YARPCanonicalNeuron *pbest = NULL;
	YARPCanonicalNeurons *canonical = objects->GetCanonical();
	assert (canonical != NULL);

	if (!notadd)
	{		
		if (!canonical->Empty())
		{
			for (int i = canonical->Begin(); i <= canonical->End(); i++)
			{
				YARPCanonicalNeuron& neuron = (*canonical)[i];
				if (!neuron.Empty())
				{
					neuron.InitCursor();
					while (!neuron.EndCursor())
					{
						YARPCanonicalData& d = neuron.Get();
						tmp.CastCopy (d.m_mask);

						// ADDED THIS.
						vss.Reset ();
						vss.Add (d.m_frame, tmp);

						//
						float correlation = vss.Compare (vs);
						if (correlation > best_match)
						{
							best_match = correlation;
							pbest = &neuron;
							best_match_neuron = i;
							printf ("entry with max %d\n", i);
						}	

						neuron.IncrementCursor();
					}
				}
				else
				{
					//
					printf ("addcanonical: an empty neuron found (it's a bug)\n");
				}
			}
		}

		printf ("best_match is %f\n", best_match);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_BEST_MATCH_NEURON);
		it.WriteFloat(float(best_match_neuron));
		SendBottle(bot);


		//
		// GUI stuff.
		//
		browser_initialized = false;
	}

	//
	//
	// prepare data.

	YARPCanonicalData u;
	u.m_mask = mask;
	u.m_frame = frame;
	u.m_starting_position = v1;
	u.m_ending_position = v2;
	u.m_starting_position_image = i1;
	u.m_ending_position_image = i2;
	u.m_pushing_direction = pushing;

	u.m_contact_frame = contact-start;
	u.Resize (stop-start+1);

	for (int i = start; i <= stop; i++)
	{
		u.m_arm_trajectory[i-start] = parm[i];		
		u.m_head_trajectory[i-start] = phead[i];		
	}

	u.m_action_number = action;

	u.m_reinforcer = reinforcer;
	tracker->GetOrientation (u.m_object_orientation, u.m_object_orientation_quality);

	it.Reset();
	it.WriteVocab(YBC_LOG_PERCEPT);
	it.WriteVocab(YBC_PERCEPT_IMAGE_POSE_SHAPE);
	it.WriteFloat(float(u.m_object_orientation));
	it.WriteFloat(float(u.m_object_orientation_quality));
	SendBottle(bot);

	// seal the current trace.
	u.Seal();

	if (!notadd)
	{
		//
		//
		// then... (after the scan).
		if (best_match > CLASSIFICATION_THR && pbest != NULL && best_match_neuron >= 0)
		{
			// we have a good match.
			// add to an existing object (a new trace).

			printf ("we have a good match, adding a trace to an existing unit\n");
			(*canonical)[best_match_neuron].Add(u);

			it.Reset();
			it.WriteVocab(YBC_LOG_PERCEPT);
			it.WriteVocab(YBC_PERCEPT_GOOD_OBJECT_MATCH);
			SendBottle(bot);
		}
		else
		{
			printf ("adding a new unit\n");

			// create a new entry object.
			int entry = canonical->AddNewObject ();	
			(*canonical)[entry].Add (u);

			it.Reset();
			it.WriteVocab(YBC_LOG_PERCEPT);
			it.WriteVocab(YBC_PERCEPT_ADDING_NEURON);
			SendBottle(bot);
		}	
	}
	else
	{
		printf ("**** addcanonical: neuron not added because stats is active\n");
		return -1;
	}

	// compute statistics.

	return 0;
}

//
// constants are defined in secret.h
//
//

int 
alloc_secret (void)
{
	tracker = new YARPFlowTracker (MAXNUMBEROFFRAMES, WIDTH, HEIGHT);
	assert (tracker != NULL);

	objects = new YARPObjectContainer();
	assert (objects != NULL);

	objects->SetCanonical (new YARPCanonicalNeurons (NEURONS, WIDTH, HEIGHT));
	assert (objects->GetCanonical() != NULL);

	objects->SetCanonicalStats (new YARPCanonicalStats (NEURONS, STATNBINS));
	assert (objects->GetCanonicalStats() != NULL);

	goodaction = new YARPGoodActionMapArray (MAXNUMBEROFACTIONS, tracker);
	assert (goodaction != NULL);

	// link the tracker to process the same storage used for recording.
	tracker->Initialize (reader->GetSequencePtr());

	return 0;
}

int
free_secret (void)
{
	if (tracker != NULL)
	{
		delete tracker;
		tracker = NULL;
	}

	if (goodaction != NULL)
	{
		delete goodaction;
		goodaction = NULL;
	}

	if (objects != NULL)
	{
		if (objects->GetCanonical() != NULL)
		{
			delete objects->GetCanonical();
			objects->SetCanonical (NULL);
		}

		if (objects->GetCanonicalStats() != NULL)
		{
			delete objects->GetCanonicalStats();
			objects->SetCanonicalStats (NULL);
		}

		delete objects;
		objects = NULL;
	}

	return 0;
}


//
// Good/bad action map
//

int
buildactionmap (int ret)
{
	if (objects->IsActive ())
	{
		printf ("*** buildactionmap: skipped because 'objects' is active\n");
		return -1;
	}

	goodaction->BuildActionMap(ret);
	return 0;
}


int 
queryactionmap (float theta, float phi, float& torso, int& action_no)
{
	goodaction->QueryActionMap (theta, phi, torso, action_no);
	return 0;
}


//
//
//
YARPImageOf<YarpPixelBGR> image;
YARPImageOf<YarpPixelBGR> out;


//
//
// 2 modes:
//	- AFFORDANCE: given an object returns the best action associated to its affordance.
//	- MIMICRY: given an object and the observer action returns the most similar action.
//		- mimicry embeds the mirror response (given object and action interprets it).
//		- embeds also an 'imposed' task descriprion.
//		- for the moment the task is imposed in order to mimic something (there are many 
//		possible choices).
//		- mimicry requires definitely something more than pure mirror response.
//		- mirror response can be obtained from the object and the observed action 
//		asking to simply replicate the direction of movement.
//
//
int 
find_object (int& bestaction)
{
	if (!objects->IsActive ())
	{
		bestaction = -1;
		printf ("find_object: the stats about objects is out of date or never computed\n");
		return -1;
	}

	if (image.GetWidth() != WIDTH)
	{
		image.Resize(WIDTH, HEIGHT);
		out.Resize(WIDTH, HEIGHT);
	}

	reader->GetCurrentFrame(image);
	if (image.GetWidth() != WIDTH || image.GetHeight() != HEIGHT)
	{
		bestaction = -1;
		printf ("find_object: no image to process, perhaps there's no connection to the frame grabber\n");
		return -1;
	}


	//
	// objects contains the description of the 'last good observed' action.
	//
	if (objects->GetLastKnownObject() != -1)
	{
		// mimicry mode.
		printf (">>>> MIMICRY MODE\n");

		YARPBottle bot;
		YARPBottleIterator it(bot);
		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_MIMICRY_MODE);
		SendBottle(bot);

		int onumber = objects->GetLastKnownObject();

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_LAST_OBJECT_OBSERVED);
		it.WriteFloat(float(onumber));
		SendBottle(bot);

		double orientation = 0, direction = 0;
		objects->GetLastKnownObjectPose (orientation, direction);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_IMAGE_POSE_DIRECTION);
		it.WriteFloat(float(orientation));
		it.WriteFloat(float(direction));
		SendBottle(bot);

		// find current orientation.
		objects->FindSimple (image, out);

		// display?
		oosd->Display (ABW_base_output_img, out);
		PtDamageWidget (ABW_base_output_img);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_CURRENT_OBJECT_OBSERVED);
		it.WriteFloat(float(objects->GetLastKnownObject()));
		SendBottle(bot);

		if (objects->GetLastKnownObject() != onumber)
		{
			printf ("**** you're not playing the mimicry game, this is a different object\n");
			objects->ResetObject();
			return -1;
		}
		

		double diff = direction - orientation;
		diff = fabs (fmod (diff, pi));
		if (diff > pi/2) diff = pi - diff;

		printf ("------- the error in the observed action was %lf\n", diff * radToDeg);
		printf ("------- obj orient %lf displ %lf\n", orientation * radToDeg, direction * radToDeg);

		// find out about the behavior of the object.
		const double affordance = objects->GetCanonicalStats()->GetPrincipalAffordance (onumber);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_OBJECT_AFFORDANCE);
		it.WriteFloat(float(affordance));
		SendBottle(bot);
		
		bool satisfy = true;
		if (fabs (affordance - diff) > pi/4)
		{
			printf ("***** -> the object did NOT satisfy its principal affordance\n");
			satisfy = false;
		}


		double oo = 0, dd = 0;
		objects->GetLastKnownObjectPose (oo, dd);

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_IMAGE_POSE_DIRECTION);
		it.WriteFloat(float(oo));
		it.WriteFloat(float(dd));
		SendBottle(bot);


		printf ("------- the current orientation is %lf\n", oo * radToDeg);


		// search for the best affordance.
		double o1 = oo+diff;
		double o2 = oo-diff;
		printf ("---- object: %d, affordance: %lf, test1: %lf, test2: %lf\n", onumber, affordance*radToDeg, o1*radToDeg, o2*radToDeg);

		if (o1 > pi/2) o1 -= pi;
		else
		if (o1 < -pi/2) o1 += pi;
		
		if (o2 > pi/2) o2 -= pi;
		else
		if (o2 < -pi/2) o2 += pi;

		double score1 = 0;
		double score2 = 0;
		int bestaction1 = objects->GetCanonicalStats()->GetBestActionGeneric (o1, score1);
		int bestaction2 = objects->GetCanonicalStats()->GetBestActionGeneric (o2, score2);

		printf ("---- score(s) %lf %lf\n", score1, score2);

		if (score1 < score2)
			bestaction = bestaction1;
		else
			bestaction = bestaction2;

		objects->ResetObject();

		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_COMPUTED_ACTION);
		it.WriteFloat(float(bestaction));
		SendBottle(bot);

		printf ("------- best action %d\n", bestaction);
	}
	else
	{
		// affordance mode.
		printf (">>>> AFFORDANCE MODE\n");
		objects->Find (image, out, bestaction);

		const int onumber = objects->GetLastKnownObject();

		YARPBottle bot;
		YARPBottleIterator it(bot);
		it.Reset();
		it.WriteVocab(YBC_LOG_PERCEPT);
		it.WriteVocab(YBC_PERCEPT_AFFORDANCE_MODE);
		SendBottle(bot);


		if (onumber != -1)
		{
			it.Reset();
			it.WriteVocab(YBC_LOG_PERCEPT);
			it.WriteVocab(YBC_PERCEPT_CURRENT_OBJECT_OBSERVED);
			it.WriteFloat(float(onumber));
			SendBottle(bot);

			double orientation = 0, direction = 0;
			objects->GetLastKnownObjectPose (orientation, direction);

			it.Reset();
			it.WriteVocab(YBC_LOG_PERCEPT);
			it.WriteVocab(YBC_PERCEPT_IMAGE_POSE_DIRECTION);
			it.WriteFloat(float(orientation));
			it.WriteFloat(float(direction));
			SendBottle(bot);

			// find out about the behavior of the object.
			const double affordance = objects->GetCanonicalStats()->GetPrincipalAffordance (onumber);

			it.Reset();
			it.WriteVocab(YBC_LOG_PERCEPT);
			it.WriteVocab(YBC_PERCEPT_OBJECT_AFFORDANCE);
			it.WriteFloat(float(affordance));
			SendBottle(bot);
		}


		// display?
		oosd->Display (ABW_base_output_img, out);
		PtDamageWidget (ABW_base_output_img);

		objects->ResetObject();
	}

	return bestaction;
}
