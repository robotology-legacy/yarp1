//
// color_segm2.cpp
//	- Aug 2002.
//
#include <sys/kernel.h>
#include <iostream.h>

#include "YARPAll.h"
#include "YARPSemaphore.h"
#include "YARPBottle.h"

// YARP stuff first.
#include "YARPImage.h"
#include "YARPFilters.h"
#include "YARPlogpolar.h"
#include "YARPSkinFilter.h"
#include "YARPBoxer.h"
#include "YARPPort.h"

#include "YARPImagePort.h"

#include <VisDMatrix.h>

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

// canonical data reader...
#include "YARPCanonicalData.h"
#include "YARPObjectContainer.h"
#include "YARPBottle.h"
#include "YARPMapCameras.h"


// these are copied from shlc... they must be consistent!
#define CANONICALFILENAME "/ai/ai/mb/cdp/src/YARP/state/canonical/canonical"
#define CSTATSFILENAME "/ai/ai/mb/cdp/src/YARP/state/canonical/stats"

const int		MAXNUMBEROFACTIONS = 4;
const int		NEURONS = 40;
const int		STATNBINS = 8;
const int		WIDTH = 128;
const int		HEIGHT = 128;



YARPInputPortOf<YARPGenericImage> in;
YARPInputPortOf<YARPGenericImage> in_wide;
YARPOutputPortOf<YARPGenericImage> out;
YARPOutputPortOf<YARPGenericImage> out_wide;

char *my_name = "/yarp/color_segm2";
char in_img_name[256];
char in_wide_name[256];
char out_img_name[256];
char out_wide_name[256];
char bottle_in_name[256];
char bottle_out_name[256];


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

	sprintf (in_img_name, "%s/i:img", my_name); 
	sprintf (in_wide_name, "%s/i:wide", my_name); 
	sprintf (out_img_name, "%s/o:img", my_name); 
	sprintf (out_wide_name, "%s/o:wide", my_name); 
	sprintf (bottle_in_name, "%s/i:bottle", my_name); 
	sprintf (bottle_out_name, "%s/o:bottle", my_name); 
}

class MessageReceiver : public YARPInputPortOf<YARPBottle>
{
protected:
	YARPBottle m_bottle;
	YARPSemaphore m_mutex;
	int m_state;

public:
	MessageReceiver (void) 
		: m_mutex(1), 		  
		  YARPInputPortOf<YARPBottle>(YARPInputPortOf::DOUBLE_BUFFERS) 
	{
		m_state = 0;
	}

	virtual void OnRead (void)
	{
		Read ();
		m_mutex.Wait();
		m_bottle = Content();

		YARPBottleIterator it (m_bottle);
		switch (it.ReadVocab())
		{
		case YBC_REACH_RANGE:
			printf ("Received a REACH_RANGE\n");
			m_state = 1;
			break;

		case YBC_REACH_OUT_OF_RANGE:
			printf ("Received a REACH_OUT_OF_RANGE\n");
			m_state = 0;
			// possibly reset the segmenter.
			break;

		}

		m_mutex.Post();
	}

	inline int GetState (void)
	{
		int ret = 0;
		m_mutex.Wait();
		ret = m_state;
		m_mutex.Post();
		return ret;
	}

	inline void SetState (int s)
	{
		m_mutex.Wait();
		m_state = s;
		m_mutex.Post();
	}
};


MessageReceiver receiver;
YARPOutputPortOf<YARPBottle> transmitter;

void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	YARPObjectContainer objects;

	objects.SetCanonical (new YARPCanonicalNeurons (NEURONS, WIDTH, HEIGHT));
	assert (objects.GetCanonical() != NULL);

	objects.SetCanonicalStats (new YARPCanonicalStats (NEURONS, STATNBINS));
	assert (objects.GetCanonicalStats() != NULL);

	printf ("Loading neurons...\n");
	objects.GetCanonical()->Load (CANONICALFILENAME);
	objects.UpdateModels();

	// build histograms for the found objects.
	printf ("there are %d objects to process\n", objects.GetCanonicalStats()->m_goodneurons);
	const int NOBJ = objects.GetCanonicalStats()->m_goodneurons;



	YARPImageOf<YarpPixelBGR> in_image;
	YARPImageOf<YarpPixelBGR> in_image_wide;

	YARPImageOf<YarpPixelBGR> out_image;
	out_image.Resize (WIDTH, HEIGHT);
	out_image.Zero ();

	in.Register (in_img_name);
	in_wide.Register (in_wide_name);
	out.Register (out_img_name);
	out_wide.Register (out_wide_name);
	receiver.Register (bottle_in_name);
	transmitter.Register (bottle_out_name);


	int counter = 0;
	int object_no = -1;
	bool first = true;
	int x = 0, y = 0;

	while (1)
	{
		in.Read();
		in_image.Refer (in.Content());

		in_wide.Read();
		in_image_wide.Refer (in_wide.Content());

		counter++;

		switch (receiver.GetState())
		{
		case 0:
		default:
			break;

		case 1:
			// if something is received from msg then start looking for object.
			objects.FindSimple (in_image, out_image);

			// if object is found, go to state 2.
			object_no = objects.GetLastKnownObject ();
			if (object_no >= 0)
			{
				first = true;
				receiver.SetState (2);
			}

			break;

		case 2:
			// if object is found keep segmenting and verify for object identity from time to time.
			objects.Segment (object_no, in_image, out_image, x, y);
			
			// map to wide angle (hopefully).
			double cx = x, cy = y;
			YARPCogMapCameras::Foveal2WideangleSquashed (cx, cy, cx, cy);
			x = int(cx);
			y = int(cy);

			YarpPixelBGR red;
			red.r = 255;
			red.g = red.b = 0;

			double betterx = x;
			double bettery = y;
			AddRectangle (in_image_wide, red, x, y, 10, 10);
			
			if (first)
			{
				// write to the tracker...
				YARPBottle bot;
				YARPBottleIterator it(bot);
				it.Reset();
				it.WriteVocab(YBC_OBJECT);
				it.WriteVocab(YBC_FOUND);
				it.WriteInt (x);
				it.WriteInt (y);
				transmitter.Content () = bot;
				transmitter.Write ();

				printf ("**** Spitting something on a YARPBottle\n");

				first = false;
			}

			if ((counter % 10) == 0)
			{
				first = true;
			}

			if ((counter % 500) == 0)
			{
				// also check whether the object is still valid.
				objects.FindObject (in_image);
				if (object_no != objects.GetLastKnownObject ())
				{
					// if !valid - object changed.
					receiver.SetState (0);
				}
				else
				{
					first = true;
				}
			}

			break;
		}

		YARPGenericImage& outImg2 = out_wide.Content ();
		outImg2.Refer (in_image_wide);
		out_wide.Write ();

		YARPGenericImage& outImg = out.Content ();
		outImg.Refer (out_image);
		out.Write ();
	}
}
