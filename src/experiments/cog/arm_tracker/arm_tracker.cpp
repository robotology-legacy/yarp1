//
// tracker - block matching
//	- Jan 2002.
//	- Jun 2002.
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
#include "YARPMapCameras.h"

#include <VisMatrix.h>
#include <Models.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

// INPUT PORTS.
YARPInputPortOf<YARPGenericImage> in;
YARPInputPortOf<TrackerMsg> in_pos;

// OUTPUT PORTS.
YARPOutputPortOf<YARPGenericImage> outt;
YARPOutputPortOf<FiveBoxesInARow> out_tracker;

char *my_name = "/yarp/arm_tracker";

char in_name[256];
char in_pos_name[256];

char outt_name[256];
char tracker_name[256];


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


#include <YARPSafeNew.h>
#include <vector.h>				// stl list
#include <CogGaze.h>

//#define _DEBUGX 1

// this is the precision of the maps.
const double PRECISION_ARM_UNIT = 40 * 40;							// arm precision (60 encoder ticks).
const double PRECISION_HEAD_UNIT = 2 * degToRad * 2 * degToRad;		// 2 deg precision for the head.

// I might want to get a response anyway but keep adding if precision is not met.
// this decides whether to give a response (i.e. a valid output).
const double HEAD_POSITION_THRESHOLD = 4*PRECISION_HEAD_UNIT;	// was 2*
const double ARM_POSITION_THRESHOLD = 4*PRECISION_ARM_UNIT;


// maps a given arm position into a given image plane pos.
class MapUnit 
{
public:
	CVisDVector x;
	CVisDVector y;
	int n;

	MapUnit ()
	{
		x.Resize(4);
		y.Resize(2);
		n = 0;
	}

	~MapUnit () {}

	inline void Add2Unit (const CVisDVector& xx, const CVisDVector& yy)
	{
		x *= n;
		y *= n;
		x += xx;
		y += yy;
		n++;
		x /= n;
		y /= n;
	}

	void Serialize (FILE *fp)
	{
		// n first.
		fprintf (fp, "%d\n", n);
		for (int i = 1; i <= 4; i++)
			fprintf (fp, "%lf ", x(i));
		fprintf (fp, "\n");

		for (i = 1; i <= 2; i++)
			fprintf (fp, "%lf ", y(i));
		fprintf (fp, "\n");
	}

	void Unserialize (FILE *fp)
	{
		fscanf (fp, "%d\n", &n);
		double tmp;
		for (int i = 1; i <= 4; i++)
		{
			fscanf (fp, "%lf ", &tmp);
			x(i) = tmp;
		}
		fscanf (fp, "\n");
		for (i = 1; i <= 2; i++)
		{
			fscanf (fp, "%lf ", &tmp);
			y(i) = tmp;
		}
		fscanf (fp, "\n");
	}
};


//
// A list of MapUnit.
//
// PRECISION is the precision on the input (threshold on the sq norm of the arm vector).
//
class HeadArmMap : public vector<MapUnit>
{
protected:
	double m_precision;
	typedef vector<MapUnit>::iterator POSITION;

	CVisDVector m_zero;
	CVisDVector m_zerox;

	POSITION m_min_index;
	double m_lasterror;

public:
	HeadArmMap () 
	{ 
		m_lasterror = PRECISION_ARM_UNIT+1;
		m_precision = PRECISION_ARM_UNIT;
		m_min_index = 0;

		m_zero.Resize(2);
		m_zero = 0;
		m_zerox.Resize(4);
		m_zerox = 0;
	}

	~HeadArmMap () {}

	void Cleanup ()
	{
		erase(begin(), end());
	}

	bool Empty(void) { return empty(); }

	inline double GetLastError (void) const { return m_lasterror; }

	// check and add unit (remember to call Nearest first).
	// x must be the same as in the call to Nearest.
	void Add (const CVisDVector& x, const CVisDVector& y)
	{
		if (m_lasterror > m_precision)
		{
			// add a complete new unit.
			MapUnit nu;
			nu.x = x;
			nu.y = y;
			nu.n = 1;
			push_back (nu);
		}
		else
		{
			// add to the existing minimum (e.g. existing unit).
			// average.
			(*m_min_index).Add2Unit (x, y);
		}
	}

	// call Nearest first. returns the nearest input vector.
	CVisDVector NearestIn (const CVisDVector& x)
	{
		if (m_min_index != 0)
		{
			return (*m_min_index).x;
		}

		return m_zerox;
	}

	double GetError (void) const { return m_lasterror; }

	CVisDVector Nearest (const CVisDVector& x)
	{
		assert (x.Length() == 4);

		if (empty())
		{
			m_lasterror = m_precision + 1;
			m_min_index = 0;
#ifdef _DEBUGX
			printf ("Nearest returns because list is empty\n");
#endif
			return m_zero;
		}
		
		POSITION index = begin();
		CVisDVector tmp(4);

		tmp = (*index).x - x;
		m_lasterror = tmp.norm2square();
		m_min_index = index;
		index++;

		while (index != end())
		{
			tmp = (*index).x - x;
			double s = tmp.norm2square();
			if (s < m_lasterror)
			{
				m_lasterror = s;
				m_min_index = index;
			}
			//
			index++;
		}

#ifdef _DEBUGX
		printf ("last error : %lf\n", m_lasterror);
#endif

		return (*m_min_index).y;
	}

	void Serialize (FILE *fp)
	{
		fprintf (fp, "%d\n", size());
		if (size() == 0)
			return;

		POSITION index = begin();

		while (index != end())
		{
			index->Serialize(fp);
			index ++;
		}

		fprintf (fp, "\n");
	}

	void Unserialize (FILE *fp)
	{
		Cleanup ();
		int sz;
		fscanf (fp, "%d\n", &sz);
		if (sz == 0)
			return;

		MapUnit x;
		for (int i = 0; i < sz; i++)
		{
			x.Unserialize (fp);
			push_back (x);
		}

		fscanf (fp, "\n");
	}
};


class BigUnit
{
public:
	CVisDVector a;
	HeadArmMap b;

	BigUnit() { a.Resize(2); a = 0; }
};


class WTAKin : public vector<BigUnit>
{
protected:
	typedef vector<BigUnit>::iterator POSITION;

	double m_precision_i;

	CVisDVector m_zero;

	POSITION m_min_index;
	double m_lasterror_i;
	bool m_good;

	CogGaze  m_cog_gaze;
	CVisDVector m_gaze;

public:
	WTAKin () 
	{
		m_lasterror_i = PRECISION_HEAD_UNIT+1;
		m_precision_i = PRECISION_HEAD_UNIT;

		m_min_index = 0;

		m_gaze.Resize (2);
		m_gaze = 0;

		m_zero.Resize(2);
		m_zero = 0;

		m_good = false;
	}

	~WTAKin () {}

	void Cleanup (void)
	{
		erase(begin(), end());
	}

	inline CVisDVector Gaze (const CVisDVector& head)
	{
		JointPos joint_pos;
		for (int i = 1; i <= 7; i++)
			joint_pos(i) = head(i);

		m_cog_gaze.Apply (joint_pos);

		CVisDVector gaze(2);
		gaze(1) = m_cog_gaze.theta_right;
		gaze(2) = m_cog_gaze.phi_right;

		return gaze;
	}

	// must have called Nearest first.
	int Add (CVisDVector& head, CVisDVector& arm, CVisDVector& img)
	{
		// head error big -> add a new unit.
		if (m_lasterror_i > m_precision_i) //HEAD_POSITION_THRESHOLD)
		{
			// add a complete new unit.
			BigUnit nu;
			nu.a = Gaze(head);
			nu.b.Add (arm, img);
			push_back (nu);

			// m_min_index is not valid (don't call add).
		}
		else
		{
			// small head error.

			// Nearest for the m_min_index has been called.
			(*m_min_index).b.Add (arm, img);
		}

		return 0;
	}

	inline bool IsGood (void) const { return m_good; }

	CVisDVector Nearest (CVisDVector& head, CVisDVector& arm)
	{
		assert (head.Length() == 7);
		assert (arm.Length() == 4);

//		m_good = true;

		// get gaze.
		m_gaze = Gaze (head);

		// search for the best gaze vector.
		if (empty())
		{
			m_lasterror_i = m_precision_i + 1;
			m_min_index = 0;

#ifdef _DEBUGX
			printf ("Nearest returns because list is empty\n");
#endif
			m_good = false;
			return m_zero;
		}
		
		POSITION index = begin();
		CVisDVector tmp(2);

		tmp = (*index).a - m_gaze;
		m_lasterror_i = tmp.norm2square();
		m_min_index = index;
		index++;

		while (index != end())
		{
			tmp = (*index).a - m_gaze;
			double s = tmp.norm2square();
			if (s < m_lasterror_i)
			{
				m_lasterror_i = s;
				m_min_index = index;
			}
			//
			index++;
		}

#ifdef _DEBUGX
		printf ("last error : %lf\n", m_lasterror_i);
#endif

		// check whether we have a good estimate.
		// if not, we need to add a new unit (not worth searching further).
		if (m_lasterror_i > m_precision_i) //HEAD_POSITION_THRESHOLD)
		{
			// if really big error don't ret anything.
			if (m_lasterror_i > HEAD_POSITION_THRESHOLD)
			{
				m_good = false;
				return m_zero;
			}
			else
			{
				m_good = true;
				tmp = (*m_min_index).b.Nearest (arm);
				return tmp;
			}
		}

		// *m_min_index is the BigUnit element.
		
		// if error test is fine then search on the internal map (arm).
		tmp = (*m_min_index).b.Nearest (arm);

		// check whether the point is already in the list.
		if ((*m_min_index).b.GetLastError() > ARM_POSITION_THRESHOLD)
		{
			// this is a really big error, ret zero.
			m_good = false;
			return m_zero;
		}

		m_good = true;
		return tmp;
	}


	void PrintV (const char *name, const CVisDVector& v)
	{
		printf ("%s : ", name);
		for (int i = 1; i <= v.Length(); i++)
			printf ("%f ", v(i));
		printf ("\n");
	}

	//
	int Serialize (const char *filename)
	{
		FILE *fp = fopen (filename, "w");
		if (fp == NULL)
		{
			printf ("can't serialize\n");
			return -1;
		}

		fprintf (fp, "size: %d\n\n", size());
		if (size() == 0)
		{
			fclose (fp);
			printf ("the main list is empty\n");
			return -1;
		}

		POSITION index = begin();
		int i = 0;

		while (index != end())
		{
			fprintf (fp, "unit: %d\n", i);
			fprintf (fp, "gaze\n");
			for (int k = 1; k <= index->a.Length(); k++)
				fprintf (fp, "%lf ", index->a(k));
			fprintf (fp, "\n");
			fprintf (fp, "map\n");
			index->b.Serialize(fp);
			index++;
			i++;
		}

		fclose (fp);
		return 0;
	}

	int Unserialize (const char *filename)
	{
		FILE *fp = fopen (filename, "r");
		if (fp == NULL)
		{
			printf ("can't unserialize\n");
			return -1;
		}

		Cleanup ();
		int sz;
		fscanf (fp, "size: %d\n\n", &sz);

		if (sz == 0)
		{
			fclose (fp);
			printf ("the main list is empty\n");
			return -1;
		}

		BigUnit x;

		for (int i = 0; i < sz; i++)
		{
			int tmp;
			fscanf (fp, "unit: %d\n", &tmp);
			fscanf (fp, "gaze\n");
			for (int k = 1; k <= x.a.Length(); k++)
			{
				double xxx = 0;
				fscanf (fp, "%lf ", &xxx);
				x.a(k) = xxx;
			}
			fscanf (fp, "\n");
			fscanf (fp, "map\n");
			x.b.Unserialize (fp);
			push_back (x);
		}

		fclose(fp);
		return 0;
	}
};

//
//
//
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

	TrackerMsg tracker_position;
	CVisDVector arm(4);
	CVisDVector head(7);
	CVisDVector img(2);
	CVisDVector predicted(2);

	// load/save, load always from this file and safe into .new.txt
	WTAKin map;
	map.Unserialize ("kinesthesia.txt");

	YarpPixelBGR red;
	red.b = red.g = 0;
	red.r = 255;

	YarpPixelBGR green;
	green.b = green.r = 0;
	green.g = 255;

	int count = 0;
	FiveBoxesInARow target_boxes;	// boxes.

	int ulx, uly, lrx, lry;
	double x, y;
	YARPCogMapCameras::Foveal2WideangleSquashed (0.0, 0.0, x, y);
	ulx = int(x);
	uly = int(y);
	YARPCogMapCameras::Foveal2WideangleSquashed (127.0, 127.0, x, y);
	lrx = int(x);
	lry = int(y);

	while (1)
	{
		// READ IMG.
		in.Read();
		in_image.Refer (in.Content());

		// READ FROM OPTIC FLOW POSITION DATA.
		in_pos.Read();
		tracker_position = in_pos.Content();
		
		arm(1) = tracker_position.aj[3];
		arm(2) = tracker_position.aj[4];
		arm(3) = tracker_position.aj[5];
		arm(4) = tracker_position.aj[6];

		for (int i = 1; i <= 7; i++)
			head(i) = tracker_position.hj[i-1];

		img(1) = tracker_position.img[0];
		img(2) = tracker_position.img[1];
		
		// IF VALID DATA, PREPARE VECTORS FOR MAP.
		if (tracker_position.valid)
		{
			predicted = map.Nearest (head, arm);
			map.Add (head, arm, img);

			AddRectangle (in_image, red, int(img(1)), int(img(2)), 15, 15);
			if (predicted(1) >= 0 && predicted(2) >= 0 && map.IsGood())
				AddRectangle (in_image, green, int(predicted(1)), int(predicted(2)), 15, 15);

			count++;
			if (count > 10)
			{
				map.Serialize ("kinesthesia.new.txt");
				count = 0;
			}

#ifdef _DEBUGX
			printf ("\n\n\n");
#endif
		}
		else
		{
			predicted = map.Nearest (head, arm);

			if (predicted(1) >= 0 && predicted(2) >= 0 && map.IsGood())
				AddRectangle (in_image, green, int(predicted(1)), int(predicted(2)), 15, 15);

#ifdef _DEBUGX
			printf ("\n\n\n");
#endif
		}

		memset (&target_boxes, 0, sizeof(FiveBoxesInARow));

		if (predicted(1) >= 0 && predicted(2) >= 0)
		{
			// scale down the box position.
			// a way to reduce the feedback gain.
			int sx = int(predicted(1)+.5);
			int sy = int(predicted(2)+.5);

			// prepare boxes for attentional system?
			//memset (&target_boxes, 0, sizeof(FiveBoxesInARow));
			// imagine size = 30
			target_boxes(0).valid = true;
			target_boxes(0).total_sal = 0;
			target_boxes(0).total_pixels = 30 * 30;
			target_boxes(0).xsum = sx * 30 * 30;
			target_boxes(0).ysum = sy * 30 * 30;
			target_boxes(0).centroid_x = sx;
			target_boxes(0).centroid_y = sy;
			target_boxes(0).xmax = sx + 15;
			target_boxes(0).xmin = sx - 15;
			target_boxes(0).ymax = sy + 15;
			target_boxes(0).ymin = sy - 15;
		}

		AddRectangle (in_image, red, (ulx+lrx)/2, (uly+lry)/2, (4*(lrx-ulx))/6, (lry-uly)/2);

		YARPGenericImage& outImg = outt.Content();
		outImg.Refer(in_image);
		outt.Write();

		out_tracker.Content () = target_boxes;
		out_tracker.Write ();
	}
}
