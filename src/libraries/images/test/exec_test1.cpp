//
//
// exec_test1.cpp
//
//	-project to be used for testing the library.
//	likely to change many many times.
//
//

#include <stdio.h>

#include "VisMatrix.h"
#include "Models.h"

#include "YARPImageServices.h"
#include "YARPTargetLocator.h"
#include "YARPCanonicalData.h"
#include "YARPSearchRotation.h"


#define SIZE 128

#define DIRNAME "..\\..\\..\\..\\state\\canonical"
#define CANONICALNAME "..\\..\\..\\..\\state\\canonical\\canonical"
#define NBINS 8
#define N_ACTIONS 4
#define SCALE 1.0


// builds model only from half of the samples. keeps the remaining for testing.
//#define HALFMODEL 1

// test for many samples.
//#define TEST 1

#define IMPROVE 1

//
int main ()
{
	printf ("Loading neurons...\n");
	YARPCanonicalNeurons canonical (40, SIZE, SIZE);
	canonical.Load (CANONICALNAME);

	YARPCanonicalStats canonical_stats (40, NBINS);
	canonical_stats.UpdateStats(canonical);

	canonical_stats.Print ();
	canonical_stats.Save ("stats.txt");

	// build histograms for the found objects.
	printf ("there are %d objects to process\n", canonical_stats.m_goodneurons);
	const int NOBJ = canonical_stats.m_goodneurons;


	// build a locator for each object.
	// 0.1 means 10% of the max (255).
	YARPColorLocator *locator;
	locator = new YARPColorLocator[NOBJ];
	for (int obj = 0; obj < NOBJ; obj++)
		// params are SIZE, bins of V, S, H and thresholds.
		locator[obj].Resize (SIZE, 15, 15, 15, 0.1, 0.1);

	// building model out of many frames.
	printf ("building model\n");
	for (obj = 0; obj < NOBJ; obj++)
	{
		YARPCanonicalNeuron& n = (*canonical_stats.m_units[obj]);

#ifdef HALFMODEL
		int cnt = 0;
		const int end = n.size()/2;
#endif

		printf ("neuron number: %d\n", obj);
		printf ("number of frames: %d\n", n.size());

		n.InitCursor ();
		locator[obj].Reset();
		while (!n.EndCursor ())
		{
			YARPCanonicalData& data = n.Get();

			YARPImageOf<YarpPixelBGR>& model = data.m_frame;
			YARPImageOf<YarpPixelMono>& mask = data.m_mask;

			locator[obj].Apply (model, mask, false);			
			n.IncrementCursor ();

#ifdef HALFMODEL 
			cnt++;
			if (cnt == end)
				break;
#endif
		}

		double ex,ey;
		locator[obj].GetExtent(ex, ey);
		printf ("extent of model x y: %lf %lf\n", ex, ey);
		char buf[512];
		sprintf (buf, "obj_histo_%d.txt", obj);
		locator[obj].SaveHistogram (buf);
	}

	YARPImageOf<YarpPixelBGR> scan;
	scan.Resize (SIZE, SIZE);

	// testing/searching.	
	YarpPixelBGR red;
	red.r = 255;
	red.g = red.b = 0;

	YARPImageOf<YarpPixelBGR> *backp;
	backp = new YARPImageOf<YarpPixelBGR>[NOBJ];
	for (obj = 0; obj < NOBJ; obj++)
		backp[obj].Resize (SIZE, SIZE);


	//
	//
	//

#ifndef TEST

	for (;;)
	{
		int neuron, frame;
		char buf[512];
		double x, y, quality;
		double max_quality = 0; 
		int max_obj = -1;
		double max_x = 0, max_y = 0;

		printf ("input required [n f] :");
		scanf ("%d %d", &neuron, &frame);
		sprintf (buf, "%s\\canonical_%04d_%04d_contact.ppm\0", DIRNAME, neuron, frame);
		YARPImageFile::Read (buf, scan);

		for (int obj = 0; obj < NOBJ; obj++)
		{
			locator[obj].BackProjectWithLuma (scan, backp[obj]);

			double ex, ey;
			locator[obj].GetExtent (ex, ey);

			ex *= SCALE;
			ey *= SCALE;

			if (locator[obj].Find (ex, ey, x, y, quality) >= 0)
			{
				printf ("object: %d location: %lf %lf q: %lf\n", obj, x, y, quality);
				if (quality > max_quality)
				{
					max_quality = quality;
					max_obj = obj;
					max_x = x;
					max_y = y;
				}
			}

			char buf[512];
			sprintf (buf,"backp%d.ppm\0",obj);
			YARPImageFile::Write (buf, backp[obj]);
		}

		double ex, ey;
		locator[max_obj].GetExtent (ex, ey);
		ex *= SCALE;
		ey *= SCALE;

#ifdef IMPROVE
		// if max_quality is not good enough then skip the following search.
		// if pointiness is not good enough then skip the following search.
		printf ("object is %d, improving position\n", max_obj);

		// try to improve the position estimation.
		double betterx = 0, bettery = 0;
		locator[max_obj].ImprovedSearch (scan, ex, ey, max_x, max_y, betterx, bettery);
#else
		double betterx = max_x;
		double bettery = max_y;
#endif
	
		printf ("object is %d, looking for orientation\n", max_obj);
	
		double angle = -1;
		double angle2 = -1;
		double ave = 0;
		double std = 0;
		locator[max_obj].GetNumberOfPoints (ave, std);

		// need to add a threshold on pointiness!
		YARPSearchRotation sr (50, 0.0);
		sr.Search (int(canonical_stats.m_neuron_numbers(max_obj+1)), canonical, scan, betterx, bettery, ave, std, angle, angle2);

		printf ("orientation: %lf\n", angle);
		int x1 = betterx + cos(angle) * 40;
		int y1 = bettery + sin(angle) * 40;
		int x2 = betterx - cos(angle) * 40;
		int y2 = bettery - sin(angle) * 40;
		AddSegment (backp[max_obj], red, x1, y1, x2, y2);
		AddRectangle (backp[max_obj], red, int(betterx+.5), int(bettery+.5), int (ex/2+.5), int (ey/2+.5));

		AddCircleOutline (backp[max_obj], red, int(max_x+.5), int(max_y+.5), 10);
		AddCircle (backp[max_obj], red, int(betterx+.5), int(bettery+.5), 5);
		YARPImageFile::Write ("backp.ppm", backp[max_obj]);

		int index = int (fmod (angle+pi, pi) / (pi / NBINS)) + 1;

		// get the best action...
		double amax = 0;
		int bestaction = -1;

		for (int i = 0; i < N_ACTIONS; i++)
		{
			double score = canonical_stats.m_actions[max_obj](i+1, index);
			if (score > amax)
			{
				bestaction = i;
				amax = score;
			}
		}

		printf ("the best action for this object in this position is %d\n", bestaction);
	}

#else

	//
	// TEST THROUGH ALL DATA SET
	//

	{
		for (int object = 1; object < NOBJ-1; object++)
		{
			double me = 0;
			double se = 0;
			int nelem = 0;
			int misses = 0;

			double x, y, quality;
			double max_quality = 0; 
			int max_obj = -1;
			double max_x = 0, max_y = 0;


			YARPCanonicalNeuron& n = (*canonical_stats.m_units[object]);

			printf ("neuron number: %d\n", object);
			printf ("number of frames: %d\n", n.size());

			n.InitCursor ();

#ifdef HALFMODEL
			for (int kk = 0; kk < n.size()/2-1; kk++)
			{
				n.IncrementCursor();
			}
#endif

			while (!n.EndCursor ())
			{
				max_quality = 0; 
				max_obj = -1;
				max_x = 0;
				max_y = 0;


				YARPCanonicalData& data = n.Get();

				YARPImageOf<YarpPixelBGR>& scan = data.m_frame;

				for (int obj = 0; obj < NOBJ; obj++)
				{
					locator[obj].BackProject (scan, backp[obj]);

					double ex, ey;
					locator[obj].GetExtent (ex, ey);

					ex *= SCALE;
					ey *= SCALE;

					if (locator[obj].Find (ex, ey, x, y, quality) >= 0)
					{
						printf ("object: %d location: %lf %lf q: %lf\n", obj, x, y, quality);
						if (quality > max_quality)
						{
							max_quality = quality;
							max_obj = obj;
							max_x = x;
							max_y = y;
						}
					}
				}


				// if classification is ok, then look for angle.
				if (max_obj == object)
				{
					// if max_quality is not good enough then skip the following search.
					// if pointiness is not good enough then skip the following search.
					
					double ex, ey;
					locator[max_obj].GetExtent (ex, ey);
					ex *= SCALE;
					ey *= SCALE;

					double angle = -1;
					double angle2 = -1;
					double ave = 0;
					double std = 0;
					locator[max_obj].GetNumberOfPoints (ave, std);

#ifdef IMPROVE
					printf ("object is %d, improving position\n", max_obj);

					// try to improve the position estimation.
					double betterx = 0, bettery = 0;
					locator[max_obj].ImprovedSearch (scan, ex, ey, max_x, max_y, betterx, bettery);
#else
					double betterx = max_x;
					double bettery = max_y;
#endif

					printf ("object is %d, looking for orientation\n", max_obj);

					// need to add a threshold on pointiness!
					YARPSearchRotation sr (50, 0.0);
					sr.Search (int(canonical_stats.m_neuron_numbers(max_obj+1)), canonical, scan, betterx, bettery, ave, std, angle, angle2);


					printf ("orientation: %lf\n", angle);


#ifdef SAVE2FILE
					int x1 = betterx + cos(angle) * 40;
					int y1 = bettery + sin(angle) * 40;
					int x2 = betterx - cos(angle) * 40;
					int y2 = bettery - sin(angle) * 40;
					AddSegment (backp[max_obj], red, x1, y1, x2, y2);
					AddRectangle (backp[max_obj], red, int(max_x+.5), int(max_y+.5), int (ex/2+.5), int (ey/2+.5));

					AddCircleOutline (backp[max_obj], red, int(max_x+.5), int(max_y+.5), 10);
					AddCircle (backp[max_obj], red, int(betterx+.5), int(bettery+.5), 5);
					YARPImageFile::Write ("backp.ppm", backp[max_obj]);

					int index = int (fmod (angle+pi, pi) / (pi / NBINS)) + 1;

					// get the best action...
					double amax = 0;
					int bestaction = -1;

					for (int i = 0; i < N_ACTIONS; i++)
					{
						double score = canonical_stats.m_actions[max_obj](i+1, index);
						if (score > amax)
						{
							bestaction = i;
							amax = score;
						}
					}

					printf ("the best action for this object in this position is %d\n", bestaction);
#endif


					nelem++;
					double diff = angle - data.m_object_orientation;
					diff = fabs (fmod (diff, pi/2));
					//if (diff > pi/2) diff = pi - diff;

					me += diff;
					se += (diff*diff);
				}
				else
				{
					printf ("bad classification\n");
					misses ++;
				}

				n.IncrementCursor ();
			}



			printf ("REPORT for object: %d\n", object);
			me /= nelem;
			se = sqrt(se / (nelem-1) - (nelem/(nelem-1) * me * me));

			printf ("Average angular error %lf and std %lf\n", me, se);
			printf ("Average angular error %lf and std %lf\n", me*180/pi, se*180/pi);

			char c;
			scanf ("%c", &c);
		}

	}

#endif



	return 0;
}

