//
// tracker - block matching
//	- Jan 2002.
//
#include <sys/kernel.h>
#include <iostream.h>
#include <time.h>

#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPColorConverter.h"
#include "YARPOpticFlowBM.h"
#include "YARPTemporalSmooth.h"
#include "YARPImageDraw.h"
#include "YARPTracker.h"

#include "YARPPort.h"
#include "YARPImagePort.h"

#include <VisMatrix.h>
#include <Models.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

// INPUT PORTS.
YARPInputPortOf<YARPGenericImage> in;
YARPInputPortOf<SinglePosData> in_pos;

// OUTPUT PORTS.
YARPOutputPortOf<YARPGenericImage> outt;
YARPOutputPortOf<FiveBoxesInARow> out_tracker;

char *my_name = "/yarp/arm_tracker";

char in_name[256];
char in_pos_name[256];

char outt_name[256];
char tracker_name[256];

// tracker stuff.
const int TEMPSIZE = 33;
const int RANGE = 15;	// 15!

// parses command line.
void ParseCommandLine(int argc, char *argv[])
{
	int i;

	for(i=1;i<argc;i++)
	{
		switch(argv[i][0])
		{
			case '-':
				switch(argv[i][1])
				{
					case 'h':
					case '-':
						cout << endl;
						cout << my_name << " argument:" << endl;
						cout << endl;
						exit(0);
						break;

					default:
						break;
				}
				break;
			case '+':
				my_name = &argv[i][1];
				break;
			default:
				break;
		}
	}

	sprintf (in_name, "%s/in", my_name); 
	sprintf (in_pos_name, "%s/inp", my_name); 

	sprintf (outt_name, "%s/outt", my_name); 
	sprintf (tracker_name, "%s/track", my_name); 
}


void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	in.Register (in_name);
	in_pos.Register (in_pos_name);

	outt.Register (outt_name);
	out_tracker.Register (tracker_name);

	YARPImageOf<YarpPixelBGR> in_image;
	YARPImageOf<YarpPixelMono> mono;

	in_image.Resize (Size, Size);
	mono.Resize (Size, Size);

	YARPTracker<YarpPixelBGR> tracker(Size,Size,TEMPSIZE,TEMPSIZE,RANGE,RANGE,1);

	//tracker.SetSpatialSmooth();
	//tracker.SetTemporalSmooth (0.5);
	tracker.SetVarianceThr (20.0);
	//tracker.SetWeightWindow (15.0);

	int sx = 0, sy = 0;
	bool trackvalid = false;
	YarpPixelBGR red;
	red.b = red.g = 0;
	red.r = 255;

	// 0 = optic flow, 1 = tracker.
	int status = 0;
	FiveBoxesInARow target_boxes;	// boxes.
	SinglePosData start_position;	// start_position of the tracker.

	while (1)
	{
		//
		in.Read();
		in_image.Refer (in.Content());

		// READ FROM OPTIC FLOW (ARM_LOCATION).
		in_pos.Read();
		start_position = in_pos.Content();
				
		// TRACKING (if valid target).
		if (start_position.valid)
		{
			sx = start_position.x;
			sy = start_position.y;

			// start a new tracker.
			tracker.Initialize (in_image, sx, sy);
			status = 1;
		}

		memset (&target_boxes, 0, sizeof(FiveBoxesInARow));
		trackvalid = false;

		if (status == 1)
		{
			if (tracker.IsTracking())
			{
				tracker.Apply (in_image, true, sx, sy, trackvalid);
			}
			else
			{
				status = 0;
			}

			if (trackvalid)
			{
				AddRectangle (in_image, red, sx, sy, TEMPSIZE/2, TEMPSIZE/2);
				AddSegment (in_image, red, sx, sy, Size/2, Size/2); 

				// scale down the box position.
				// a way to reduce the feedback gain.
				sx = (sx - Size/2) / 2 + Size/2;
				sy = (sy - Size/2) / 2 + Size/2;

				// prepare boxes for attentional system?
				//memset (&target_boxes, 0, sizeof(FiveBoxesInARow));
				target_boxes(0).valid = true;
				target_boxes(0).total_sal = 0;
				target_boxes(0).total_pixels = TEMPSIZE * TEMPSIZE;
				target_boxes(0).xsum = sx * TEMPSIZE * TEMPSIZE;
				target_boxes(0).ysum = sy * TEMPSIZE * TEMPSIZE;
				target_boxes(0).centroid_x = sx;
				target_boxes(0).centroid_y = sy;
				target_boxes(0).xmax = sx + TEMPSIZE/2;
				target_boxes(0).xmin = sx - TEMPSIZE/2;
				target_boxes(0).ymax = sy + TEMPSIZE/2;
				target_boxes(0).ymin = sy - TEMPSIZE/2;
			}
		}

		YARPGenericImage& outImg = outt.Content();
		outImg.Refer(in_image);
		outt.Write();

		out_tracker.Content () = target_boxes;
		out_tracker.Write ();
	}
}


