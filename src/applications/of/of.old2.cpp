//
// of block matching test
//	- Oct 2001.
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

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;
YARPOutputPortOf<YARPGenericImage> outs;
YARPOutputPortOf<YARPGenericImage> outt;
YARPInputPortOf<ArmJoints> armpos;
YARPInputPortOf<JointPos> headpos;
YARPOutputPortOf<FiveBoxesInARow> out_tracker;

char *my_name = "/yarp/of";

char in_name[256];
char out_name[256];
char outs_name[256];
char outt_name[256];
char armpos_name[256];
char headpos_name[256];
char tracker_name[256];

// more global stuff.
const int MAXFLOW = 4;
const int BLOCKSIZE = 8;
const int LIMIT = MAXFLOW+1;
const int NCLUSTERS = 2*MAXFLOW+1;

// tracker stuff.
const int TEMPSIZE = 33;
const int RANGE = 7;

// prototypes.
void WriteBigPixel (YARPImageOf<YarpPixelMono>& out, int x, int y, unsigned char color, int size);

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
	sprintf (out_name, "%s/out", my_name); 
	sprintf (outs_name, "%s/outs", my_name); 
	sprintf (outt_name, "%s/outt", my_name); 
	sprintf (armpos_name, "%s/armp", my_name); 
	sprintf (headpos_name, "%s/headp", my_name); 
	sprintf (tracker_name, "%s/track", my_name); 
}


void WriteBigPixel (YARPImageOf<YarpPixelMono>& out, int x, int y, unsigned char color, int size)
{
	for (int i = y; i < y+size; i++)
		for (int j = x; j < x+size; j++)
		{
			out (j,i) = color;
		}
}


void CorrelateJoint (int *vx, int *vy, int ox, int oy, int *cmap, CVisDVector& jspeed)
{
	const double FLOWTHR = 1.0;
	const double JNTTHR = 50.0;

	for (int i = 1; i < oy-1; i++)
	{
		for (int j = 1; j < ox-1; j++)
		{
			int in = i*ox+j;
			if (vx[in] < LIMIT && vy[in] < LIMIT)
			{
				double mag = sqrt(vx[in] * vx[in] + vy[in] * vy[in]);
				if (mag > FLOWTHR)
				{
					if (fabs(jspeed(1)) > JNTTHR ||
						fabs(jspeed(2)) > JNTTHR ||
						fabs(jspeed(3)) > JNTTHR ||
						fabs(jspeed(4)) > JNTTHR ||
						fabs(jspeed(5)) > JNTTHR ||
						fabs(jspeed(6)) > JNTTHR)
					{
						(cmap[in]) ++;
						if (cmap[in] > 255)
							cmap[in] = 255;
					}
					else
					{
						(cmap[in]) --;
						if (cmap[in] < 0)
							(cmap[in]) = 0;
					}
				}
				else
				{
					cmap[in] -=2;
					if (cmap[in] < 0)
						(cmap[in]) = 0;
				}
			}
			else
			{
				cmap[in] -=4;
				if (cmap[in] < 0)
					(cmap[in]) = 0;
			}
		}
	}
}

inline int sgn(double v) { return (v>0)?1:-1; }

void CorrelateJoint2 (bool headmoves, int *vx, int *vy, int ox, int oy, int *cmap, CVisDVector& jspeed)
{
	const double FLOWTHR = 1.0;
	const double JNTTHR = 50.0;

	for (int i = 1; i < oy-1; i++)
	{
		for (int j = 1; j < ox-1; j++)
		{
			int in = i*ox+j;
			if (vx[in] < LIMIT && vy[in] < LIMIT && !headmoves)
			{
				double mag = sqrt(vx[in] * vx[in] + vy[in] * vy[in]);
				if (mag > FLOWTHR)
				{
					if (fabs(jspeed(1)) > JNTTHR ||
						//fabs(jspeed(2)) > JNTTHR ||
						fabs(jspeed(3)) > JNTTHR ||
						fabs(jspeed(4)) > JNTTHR ||
						fabs(jspeed(5)) > JNTTHR ||
						fabs(jspeed(6)) > JNTTHR)
					{
//						if (sgn(vx[in]) == sgn(jspeed(1)) ||
						if (sgn(vx[in]) == -sgn(jspeed(3)) ||
							sgn(vx[in]) == sgn(jspeed(4)) ||
							sgn(vx[in]) == -sgn(jspeed(5)))
//							sgn(vx[in]) == sgn(jspeed(6)))
						{
							cmap[in]++;
							if (cmap[in] > 255)
								cmap[in] = 255;
						}
						else
						{
							cmap[in]--;
							if (cmap[in] < 0)
								(cmap[in]) = 0;
						}
					}
					else
					{
						(cmap[in]) --;
						if (cmap[in] < 0)
							(cmap[in]) = 0;
					}
				}
				else
				{
					cmap[in] -=1;	// it was -=2
					if (cmap[in] < 0)
						(cmap[in]) = 0;
				}
			}
			else
			{
				cmap[in] -=4;
				if (cmap[in] < 0)
					(cmap[in]) = 0;
			}
		}
	}
}


void Convert2Img (YARPImageOf<YarpPixelMono>& out, int* cmap, int ox, int oy)
{
	// convert histogram to image.
	out.Zero();
	const int pixsize = out.GetWidth() / ox;

	int x = 0, y = 0, max = 0;
	for (int i = 0; i < oy; i++)
		for (int j = 0; j < ox; j++)
		{
			unsigned char tmp;
			if (*cmap > 255)
				tmp = 255;
			else
				tmp = (unsigned char)(*cmap);

			WriteBigPixel (out, j * pixsize, i * pixsize, tmp, pixsize);
			if (*cmap > max)
			{
				max = *cmap;
				x = j;
				y = i;
			}
			cmap++;
		}

	// plot the max! (this would distort the comp of the average).
	//if (max != 0)
	//	WriteBigPixel (out, x * pixsize, y * pixsize, 255, pixsize);
}

class RegionGrowing
{
protected:
	enum { MINNUMELEM = 3 };

	int sx, sy;			// size.
	int last_tag;		// last/current tag no.
	int blocksize;		// block size.

	int *numelem;		// num of elements per region.
	int *tagged;		// tagged image.

	YARPImageOf<YarpPixelMono> *img;
	int threshold;		// on the backprojection.

	int *tx, *ty;
	int *vx, *vy;

	int maxx, maxy;
	int maxval;
	int whoismax;		// tag the maximum belongs to.

	int maxcounter;		// count how stable is the max position.
	int oldmx, oldmy;	// old position.

public:
	RegionGrowing (int sizex, int sizey, int bsize, int thr)
	{
		numelem = NULL;
		sx = sizex;
		sy = sizey;
		last_tag = 0;
		blocksize = bsize;

		numelem = new int[sx * sy];
		assert (numelem != NULL);
		tagged = new int[sx * sy];
		assert (tagged != NULL);

		threshold = thr;
		img = NULL;

		tx = new int[sx * sy];
		ty = new int[sx * sy];
		vx = new int[sx * sy];
		vy = new int[sx * sy];
		assert (tx != NULL);
		assert (ty != NULL);
		assert (vx != NULL);
		assert (vy != NULL);

		maxx = maxy = maxval = 0;
		whoismax = 0;

		maxcounter = 0;
		oldmx = oldmy = 0;
	}

	~RegionGrowing ()
	{
		if (numelem != NULL)
			delete[] numelem;
		if (tagged != NULL)
			delete[] tagged;

		if (tx != NULL)
			delete[] tx;
		if (ty != NULL)
			delete[] ty;
		if (vx != NULL)
			delete[] vx;
		if (vy != NULL)
			delete[] vy;
	}

	void RecRegionGrowing (int i, int j)
	{
		if (tagged[i*sx+j] != 0 || (*img)(j*blocksize,i*blocksize) < threshold) 
			return;
		else
		{
			tagged[i*sx+j] = last_tag;
			numelem[last_tag]++;
		}

		if (j + 1 < sx-1)
		{
			RecRegionGrowing (i, j+1);
			if (i - 1 >= 1)
			{
				RecRegionGrowing (i - 1, j + 1);
				RecRegionGrowing (i - 1, j);
			}

			if (i + 1 < sy-1)
			{
				RecRegionGrowing (i+1, j+1);
				RecRegionGrowing (i+1, j);
			}
		}

		if (j - 1 >= 1)
		{
			RecRegionGrowing (i, j-1);
			if (i - 1 >= 1)
				RecRegionGrowing (i-1,j-1);

			if (i + 1 < sy-1)
				RecRegionGrowing (i+1,j-1);
		}

		return;
	}

	void TagImage (YARPImageOf<YarpPixelMono> *himg)
	{
		memset (tagged, 0, sizeof(int) * sx * sy);
		memset (numelem, 0, sizeof(int) * sx * sy);
		img = himg;

		last_tag = 0;

		for (int i = 1; i < sy-1; i++)
		{
			for (int j = 1; j < sx-1; j++)
			{
				if (tagged[i*sx+j] == 0)
				{
					last_tag++;
					RecRegionGrowing (i, j);
				}
			}
		}

		// find the max and store it somewhere.
		unsigned char max = 0;
		int mx = 0,my = 0;
		for (i = 1; i < sy-1; i++)
			for (int j = 1; j < sx-1; j++)
			{
				unsigned char tmp = (*img)(j*blocksize,i*blocksize);
				if (tmp > max)
				{
					max = tmp;
					mx = j;
					my = i;
				}
			}

		oldmx = maxx;
		oldmy = maxy;

		maxx = mx;
		maxy = my;
		maxval = max;
	}

	void DrawRegions (YARPImageOf<YarpPixelMono>& out, int *vxx, int*vyy)
	{
		memset (tx, 0, sizeof(int)*sx*sy);
		memset (ty, 0, sizeof(int)*sx*sy);
		memset (vx, 0, sizeof(int)*sx*sy);
		memset (vy, 0, sizeof(int)*sx*sy);
	
		YarpPixelMono WHITE = 255;

		int sum;
		
		whoismax = 0;

		for (int tag = 1; tag <= last_tag; tag++)
		{
			if (numelem[tag] >= MINNUMELEM)
			{
				sum = 0;
				for (int i = 1; i < sy-1; i++)
					for (int j = 1; j < sx-1; j++)
					{
						if (tagged[i*sx+j] == tag)
						{
							tx[tag] += (j*blocksize * (*img)(j*blocksize,i*blocksize));
							ty[tag] += (i*blocksize * (*img)(j*blocksize,i*blocksize));
							vx[tag] += (vxx[i*sx+j] * (*img)(j*blocksize,i*blocksize));
							vy[tag] += (vyy[i*sx+j] * (*img)(j*blocksize,i*blocksize));
							sum += (*img)(j*blocksize,i*blocksize);
							if (i == maxy && j == maxx)
							{
								whoismax = tag;
							}
						}
					}

				if (sum != 0)
				{
					tx[tag] /= sum;
					ty[tag] /= sum;
					vx[tag] /= sum;
					vy[tag] /= sum;
					AddCircleOutline (*img, WHITE, int(tx[tag]), int(ty[tag]), 20);
					AddSegment (*img, WHITE, int(tx[tag]), int(ty[tag]), int(tx[tag]+5*vx[tag]+.5), int(ty[tag]+5*vy[tag]+.5)); 
				}
				else
				{
					tx[tag] = img->GetWidth() / 2;
					ty[tag] = img->GetHeight() / 2;
					vx[tag] = 0;
					vy[tag] = 0;
					numelem[tag] = 0;
					whoismax = 0;
				}
			}
		}

		// plot the position of the max.
		if (whoismax != 0)
			WriteBigPixel (*img, maxx*blocksize, maxy*blocksize, 255, blocksize);

		double d = sqrt((maxx-oldmx)*(maxx-oldmx)+(maxy-oldmy)*(maxy-oldmy));
		if (d >= 3.0 || whoismax == 0)
		{
			maxcounter = 0;
		}
		else
		{
			maxcounter++;
		}
	}

	int GetLastMaxAveraged (int& mx, int& my)
	{
		if (whoismax == 0)
		{
			printf ("not a valid maximum\n");
			return -1;
		}

		mx = tx[whoismax];
		my = ty[whoismax];
		//mx = maxx * blocksize + blocksize/2;
		//my = maxy * blocksize + blocksize/2;
		return 0;
	}

	int GetLastMaxPlain (int& mx, int& my)
	{
		if (whoismax == 0)
		{
			printf ("not a valid maximum\n");
			return -1;
		}

		mx = maxx * blocksize;
		my = maxy * blocksize;
		return 0;
	}

	bool StableMax (int nframes) const
	{
		return (maxcounter > nframes)?true:false;
	}
};


FILE *fp = NULL;
void OpenLog ()
{
	fp = fopen ("logfile.log", "w");
	assert (fp != NULL);
}

void WriteLog (int *vx, int *vy, int ox, int oy, CVisDVector& v)
{
	double mag = 0;
	for (int i = 4; i < oy-4; i++)
		for (int j = 4; j < ox-4; j++)
		{
			int in = i*ox+j;
			if (vx[in] < LIMIT && vy[in] < LIMIT)
			{
				//mag = sqrt(vx[in]*vx[in]+vy[in]*vy[in]);
				mag = vx[in];
			}
			else
				mag = 0;

			fprintf (fp, "%f ", mag);
		}

	for (i = 1; i <= v.Length(); i++)
		fprintf (fp, "%f ", v(i));

	fprintf (fp, "\n");
}


void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	in.Register (in_name);
	out.Register (out_name);
	outs.Register (outs_name);
	outt.Register (outt_name);
	armpos.Register (armpos_name);
	headpos.Register (headpos_name);
	out_tracker.Register (tracker_name);

	YARPImageOf<YarpPixelBGR> in_image;
	YARPImageOf<YarpPixelMono> out_image;
	YARPImageOf<YarpPixelMono> mono;

	in_image.Resize (Size, Size);
	out_image.Resize (Size, Size);
	mono.Resize (Size, Size);

	YARPOpticFlowBM<YarpPixelMono> of(Size,Size,BLOCKSIZE,MAXFLOW,1);
	YARPTracker<YarpPixelBGR> tracker(Size,Size,TEMPSIZE,TEMPSIZE,RANGE,RANGE,1);	

	//tracker.SetSpatialSmooth();
	tracker.SetTemporalSmooth (0.3);
	tracker.SetVarianceThr (500.0);

	int ox, oy;
	of.GetNBlocks (ox, oy);

	RegionGrowing regiongrow (ox, oy, BLOCKSIZE, 10); //15);

	// 
	int *vx, *vy;
	vx = new int[ox*oy];
	vy = new int[ox*oy];
	assert (vx != NULL && vy != NULL);

	of.SetSpatialSmooth();
	of.SetVarianceThr (100.0);

	ArmJoints old_position;
	ArmJoints cur_position;
	JointPos old_positionh;
	JointPos cur_positionh;

	// process first frame.
	in.Read();
	
	armpos.Read();
	headpos.Read();
	old_position = armpos.Content();
	old_positionh = headpos.Content();

	clock_t m_prev_cycle = clock();
	clock_t now;

	in_image.Refer (in.Content());
	
	YARPColorConverter::RGB2Grayscale (in_image, mono);
	of.Initialize (mono);

	CVisDVector velocity(6);
	CVisDVector ovelocity(6);
	velocity = 0;
	ovelocity = 0;

	CVisDVector velocityh(7);
	CVisDVector ovelocityh(7);
	velocityh = 0;
	ovelocityh = 0;

	const double l = 0.5;
	double oldfx = 0, oldfy = 0;
	int last_cycle = 0;

	int *correlationmap;
	correlationmap = new int[ox*oy];
	assert (correlationmap != NULL);
	memset (correlationmap, 0, sizeof(int)*ox*oy);

	int sx = 0, sy = 0;
	bool trackvalid = false;
	YarpPixelBGR red;
	red.b = red.g = 0;
	red.r = 255;

	// 0 = optic flow, 1 = tracker.
	int status = 0;
	FiveBoxesInARow target_boxes;	// boxes.

	while (1)
	{
		in.Read();
		armpos.Read();
		headpos.Read();

		now = clock();
		last_cycle = ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
		m_prev_cycle = now;

		// COMPUTE JOINT SPEED.
		cur_position = armpos.Content();
		cur_positionh = headpos.Content();

		for (int i = 3; i <= 8; i++)
		{
			velocity(i-2) = (cur_position.j[i] - old_position.j[i]) * 1000.0 / last_cycle;
			velocity(i-2) = l * velocity(i-2) + (1-l) * ovelocity(i-2);
		}			
		
		for (i = 1; i <= 7; i++)
		{
			velocityh(i) = (cur_positionh(i) - old_positionh(i)) * 1000.0 / last_cycle;
			velocityh(i) = l * velocityh(i) + (1-l) * ovelocityh(i);
		}

		old_position = cur_position;
		ovelocity = velocity;

		old_positionh = cur_positionh;
		ovelocityh = velocityh;

		in_image.Refer (in.Content());

		// OPTIC FLOW STUFF.
		YARPColorConverter::RGB2Grayscale (in_image, mono);
		of.Apply (mono, out_image, vx, vy);

		bool headmoves = false;
		for (i = 1; i <= 7; i++)
		{
			if (fabs(velocityh(i)) > 500.0)
				headmoves = true;
		}

		//CorrelateJoint (vx, vy, ox, oy, correlationmap, velocity);
		CorrelateJoint2 (headmoves, vx, vy, ox, oy, correlationmap, velocity);
		Convert2Img (mono, correlationmap, ox, oy);

		regiongrow.TagImage (&mono);
		regiongrow.DrawRegions (mono, vx, vy);


		// TRACKING.
		// if stable max && !tracking already then start tracker.
		if (regiongrow.StableMax(10)) // && !tracker.IsTracking())
		{
			if (regiongrow.GetLastMaxAveraged (sx, sy) == 0)
			{
				// start a new tracker.
				tracker.Initialize (in_image, sx, sy);
				status = 1;
			}
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

				// prepare boxes for attentional system?
				memset (&target_boxes, 0, sizeof(FiveBoxesInARow));
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

		YARPGenericImage& outImg1 = out.Content();
		outImg1.Refer(out_image);
		out.Write();

		YARPGenericImage& outImg2 = outs.Content();
		outImg2.Refer(mono);
		outs.Write();

		YARPGenericImage& outImg3 = outt.Content();
		outImg3.Refer(in_image);
		outt.Write();

		out_tracker.Content () = target_boxes;
		out_tracker.Write ();
	}
}



#if 0
		// two STATES...
		switch (status)
		{
		case 0:
			{
				YARPColorConverter::RGB2Grayscale (in_image, mono);
				of.Apply (mono, out_image, vx, vy);

				bool headmoves = false;
				for (i = 1; i <= 7; i++)
				{
					if (fabs(velocityh(i)) > 500.0)
						headmoves = true;
				}

				//CorrelateJoint (vx, vy, ox, oy, correlationmap, velocity);
				CorrelateJoint2 (headmoves, vx, vy, ox, oy, correlationmap, velocity);
				Convert2Img (mono, correlationmap, ox, oy);

				regiongrow.TagImage (&mono);
				regiongrow.DrawRegions (mono, vx, vy);

				// start tracker?
				// if stable max && !tracking already then start tracker.
				if (regiongrow.StableMax(20) && !tracker.IsTracking())
				{
					if (regiongrow.GetLastMaxAveraged (sx, sy) == 0)
					{
						// start a new tracker.
						tracker.Initialize (in_image, sx, sy);
						status = 1;
					}
				}
			}
			break;

		case 1:
			{
				if (tracker.IsTracking())
				{
					tracker.Apply (in_image, true, sx, sy, trackvalid);
				}
				else
				{
					YARPColorConverter::RGB2Grayscale (in_image, mono);
					of.Initialize (mono);
					memset (correlationmap, 0, sizeof(int)*ox*oy);
					status = 0;
				}

				if (trackvalid)
				{
					AddRectangle (in_image, red, sx, sy, TEMPSIZE/2, TEMPSIZE/2);
					AddSegment (in_image, red, sx, sy, Size/2, Size/2); 
				}
			}
			break;
		}
#endif