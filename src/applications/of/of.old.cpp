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

#include "YARPPort.h"
#include "YARPImagePort.h"

#include <VisMatrix.h>
#include <Models.h>

#include <conf/lp_layout.h>
#include <conf/tx_data.h>

YARPInputPortOf<YARPGenericImage> in;
YARPOutputPortOf<YARPGenericImage> out;

#if 0
YARPOutputPortOf<YARPGenericImage> phisto;
YARPOutputPortOf<YARPGenericImage> gray;
YARPOutputPortOf<SinglePosData> avel;
#endif

YARPInputPortOf<JointPos> speed;

char *my_name = "/yarp/of";

char in_name[256];
char out_name[256];
char speed_name[256];

#if 0
char phisto_name[256];
char avel_name[256];
char gray_name[256];
#endif

// more global stuff.
const int MAXFLOW = 4;
const int BLOCKSIZE = 8;
const int LIMIT = MAXFLOW+1;
const int NCLUSTERS = 2*MAXFLOW+1;


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
	sprintf (phisto_name, "%s/histo", my_name); 
	sprintf (gray_name, "%s/gray", my_name); 

	sprintf (speed_name, "%s/speed", my_name); 
	sprintf (avel_name, "%s/ivel", my_name); 
}


void WriteBigPixel (YARPImageOf<YarpPixelMono>& out, int x, int y, unsigned char color, int size)
{
	for (int i = y; i < y+size; i++)
		for (int j = x; j < x+size; j++)
		{
			out (j,i) = color;
		}
}

//
void BuildHistogram (CStaticLut<int>& histo, int *vx, int *vy, int sizex, int sizey)
{
	histo = 0;

	// remove border flow.
	for (int j = 1; j < sizey-1; j++)
	{
		for (int i = 1; i < sizex-1; i++)
		{
			if (vx[j*sizex+i] < LIMIT && vy[j*sizex+i] < LIMIT)
			{
				double x = vx[j*sizex+i];
				double y = vy[j*sizex+i];
				histo(x,y) ++;
				if (histo (x, y) > histo.GetMaximum ())
				{
					histo.GetMaximum () = histo (x, y);
				}
			}
		}
	}
}

void ConvertHistogram2Img (YARPImageOf<YarpPixelMono>& out, CStaticLut<int>& histo)
{
	// convert histogram to image.
	out.Zero();
	int * ptr = histo.GetArrayPtr ()[0];

	unsigned char **ip = (unsigned char **)out.GetArray();

	const int width = histo.GetWidth ();
	const int height = histo.GetHeight ();
	double max = histo.GetMaximum();

	const int pixsize = out.GetWidth() / width;

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			int tmp = *ptr++;
			tmp = int (tmp / max * 255.0 + .5);
			if (tmp > 255) 
				tmp = 255;
			WriteBigPixel (out, j * pixsize, i * pixsize, tmp, pixsize);
		}
}

void BackprojectHistogram (YARPImageOf<YarpPixelMono>& hout, CStaticLut<int>& flow_histo, int *vx, int *vy, int ox, int oy)
{
	const double max = 10; //flow_histo.GetMaximum();

	for (int i = 1; i < oy-1; i++)
	{
		for (int j = 1; j < ox-1; j++)
		{
			double x = vx[i*ox+j];
			double y = vy[i*ox+j];
			if (x < LIMIT && y < LIMIT)
			{
				if (flow_histo (x,y) > 0)
				{
					int tmp = flow_histo(x,y);
					tmp = int (tmp / max * 255.0 + .5);
					if (tmp > 255) 
						tmp = 255;
					WriteBigPixel (hout, j * BLOCKSIZE, i * BLOCKSIZE, tmp, BLOCKSIZE);
				}
				else
				if (flow_histo (x,y) == -1)
				{
					WriteBigPixel (hout, j * BLOCKSIZE, i * BLOCKSIZE, 255, BLOCKSIZE);
				}
				else
					WriteBigPixel (hout, j * BLOCKSIZE, i * BLOCKSIZE, 0, BLOCKSIZE);
			}
			else
				WriteBigPixel (hout, j * BLOCKSIZE, i * BLOCKSIZE, 0, BLOCKSIZE);
		}
	}
}

bool FindTarget (YARPImageOf<YarpPixelMono>& hout, int bsize, double *tx, double *ty, int thr, int *vx, int *vy, int ox, int oy, double *avex, double *avey)
{
	*tx = *ty = 0;
	*avex = *avey = 0;
	int count = 0;
	int sum = 0;

	assert (oy == hout.GetHeight()/bsize && ox == hout.GetWidth()/bsize);

	for (int i = 0; i < oy; i++)
		for (int j = 0; j < ox; j++)
		{
			if (hout(j*bsize,i*bsize) > thr)
			{
				*tx += (j*bsize * hout(j*bsize,i*bsize));
				*ty += (i*bsize * hout(j*bsize,i*bsize));
				*avex += (vx[i*ox+j] * hout(j*bsize,i*bsize));
				*avey += (vy[i*ox+j] * hout(j*bsize,i*bsize));
				count ++;
				sum += hout(j*bsize,i*bsize);
			}
		}

	if (count > 3 && sum != 0)
	{
		*tx /= sum;
		*ty /= sum;
		*avex /= sum;
		*avey /= sum;
		return true;
	}
	else
	{
		*tx = hout.GetWidth() / 2;
		*ty = hout.GetHeight() / 2;
		*avex = 0;
		*avey = 0;
		return false;
	}
}

const int GRAYTHR=50;

void Filter(YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelMono>& dest)
{
  SatisfySize(src,dest);
  for (int x=0; x<src.GetWidth(); x++)
    {
      for (int y=0; y<src.GetHeight(); y++)
	{
	  YarpPixelBGR& pix = src(x,y);
	  float r = pix.r;
	  float g = pix.g;
	  float b = pix.b;
	  float gray  = 0;
	  int mask = 0;
	  if (r>GRAYTHR && g>GRAYTHR && b>GRAYTHR)
	    {
	      gray = fabs(1-g/r) + fabs(1-b/r);
	      mask = (r < g * 1.1) && (r > g * 0.95) && (r < b * 1.2) && (r > b);
	      int saturation = (r >= 254);
	      mask = mask || saturation;
	    }
	  dest(x,y) = mask*255;
	}
    }
}

void Filter2 (YARPImageOf<YarpPixelBGR>& src, YARPImageOf<YarpPixelMono>& dest, int ox, int oy)
{
	for (int i = 0; i < oy; i++)
	{
		for (int j = 0; j < ox; j++)
		{
			int x = j * BLOCKSIZE;
			int y = i * BLOCKSIZE;
			int r = 0, g = 0, b = 0;
			for (int k = y; k < y + BLOCKSIZE; k++)
				for (int l = x; l < x + BLOCKSIZE; l++)
				{
					r += (src(l, k).r);
					g += (src(l, k).g);
					b += (src(l, k).b);
				}

			r /= (BLOCKSIZE * BLOCKSIZE);
			g /= (BLOCKSIZE * BLOCKSIZE);
			b /= (BLOCKSIZE * BLOCKSIZE);

			int mask = 0;
			if (r>GRAYTHR && g>GRAYTHR && b>GRAYTHR)
			{
				//gray = fabs(1-g/r) + fabs(1-b/r);
				mask = (r < g * 1.1) && (r > g * 0.95) && (r < b * 1.2) && (r > b);
				int saturation = (r >= 254);
				mask = mask || saturation;
			}

			//dest(x,y) = mask*255;
			WriteBigPixel (dest, j*BLOCKSIZE, i*BLOCKSIZE, mask*255, BLOCKSIZE);
		}
	}
}

class RegionGrowing
{
protected:
	int sx, sy;			// size.
	int last_tag;		// last/current tag no.
	int blocksize;		// block size.

	int *numelem;		// num of elements per region.
	int *tagged;		// tagged image.

	YARPImageOf<YarpPixelMono> *img;
	int threshold;		// on the backprojection.

	int *tx, *ty;
	int *vx, *vy;

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
	}

	void DrawRegions (YARPImageOf<YarpPixelMono>& out, int *vxx, int*vyy)
	{
		memset (tx, 0, sizeof(int)*sx*sy);
		memset (ty, 0, sizeof(int)*sx*sy);
		memset (vx, 0, sizeof(int)*sx*sy);
		memset (vy, 0, sizeof(int)*sx*sy);
	
		YarpPixelMono WHITE = 255;

		int sum;

		for (int tag = 1; tag <= last_tag; tag++)
		{
			if (numelem[tag] > 5)
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
				}
			}
		}
	}
};


void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	in.Register (in_name);
	out.Register (out_name);
	speed.Register (speed_name);

	YARPImageOf<YarpPixelBGR> in_image;
	YARPImageOf<YarpPixelMono> hout;
	YARPImageOf<YarpPixelMono> mono;

	in_image.Resize (Size, Size);
	out_image.Resize (Size, Size);
	hout.Resize (Size, Size);
	mono.Resize (Size, Size);

	YARPOpticFlowBM<YarpPixelMono> of(Size,Size,BLOCKSIZE,MAXFLOW,1);
	
	int ox, oy;
	of.GetNBlocks (ox, oy);

	// 
	int *vx, *vy;
	vx = new int[ox*oy];
	vy = new int[ox*oy];
	assert (vx != NULL && vy != NULL);

	of.SetSpatialSmooth();
	of.SetVarianceThr (100.0);

	JointPos old_position;
	JointPos cur_position;

	// process first frame.
	in.Read();
	
	speed.Read();
	old_position = speed.Content();

	clock_t m_prev_cycle = clock();
	clock_t now;
	int last_cycle = 0;

	in_image.Refer (in.Content());
	
	YARPColorConverter::RGB2Grayscale (in_image, mono);
	of.Initialize (mono);

	CVisDVector vspeed(7);
	CVisDVector oldspeed(7);
	bool first_cycle = true;

	const double l = 0.5;
	double oldfx = 0, oldfy = 0;

	while (1)
	{
		in.Read();
		speed.Read();

		now = clock();
		last_cycle = ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
		m_prev_cycle = now;

		in_image.Refer (in.Content());
		YARPColorConverter::RGB2Grayscale (in_image, mono);

		cur_position = speed.Content();

		of.Apply (mono, out_image, vx, vy);

		double jvx = (cur_position.j3 - old_position.j3) * 1000.0 / last_cycle;
		double jvy = (cur_position.j1 - old_position.j1) * 1000.0 / last_cycle;
		
		jvx = l * jvx + (1-l) * oldfx;
		jvy = l * jvy + (1-l) * oldfy;

		old_position = cur_position;
		oldfx = jvx;
		oldfy = jvy;

		YARPGenericImage& outImg = out.Content();
		outImg.Refer(mono);
		out.Write();
	}
}







#if 0
void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	in.Register (in_name);
	out.Register (out_name);
	phisto.Register (phisto_name);
	gray.Register (gray_name);

	speed.Register (speed_name);
	avel.Register (avel_name);

	YARPImageOf<YarpPixelBGR> in_image;
	YARPImageOf<YarpPixelMono> hout;

	YARPImageOf<YarpPixelMono> out_image;
	YARPImageOf<YarpPixelMono> mono;
	YARPImageOf<YarpPixelMono> grayfiltered;

	in_image.Resize (Size, Size);
	out_image.Resize (Size, Size);
	hout.Resize (Size, Size);
	mono.Resize (Size, Size);
	grayfiltered.Resize (Size, Size);

	YARPTemporalSmooth smooth;
	mono.Zero ();
	smooth.Init (mono, 0.3);

	YARPOpticFlowBM<YarpPixelMono> of(Size,Size,BLOCKSIZE,MAXFLOW,1);
	
	int ox, oy;
	of.GetNBlocks (ox, oy);

	RegionGrowing regiongrow (ox, oy, BLOCKSIZE, 10);

	// 
	int *vx, *vy;
	vx = new int[ox*oy];
	vy = new int[ox*oy];
	assert (vx != NULL && vy != NULL);

	CStaticLut<int> flow_histo (-MAXFLOW, MAXFLOW, -MAXFLOW, MAXFLOW, NCLUSTERS, NCLUSTERS);

	of.SetSpatialSmooth();
	of.SetVarianceThr (100.0);

	JointPos old_position;
	JointPos cur_position;

	// process first frame.
	in.Read();
	
	speed.Read();
	old_position = speed.Content();

	clock_t m_prev_cycle = clock();
	clock_t now;
	int last_cycle = 0;

	in_image.Refer (in.Content());
	
	YARPColorConverter::RGB2Grayscale (in_image, mono);
	of.Initialize (mono);

	CVisDVector vspeed(7);
	CVisDVector oldspeed(7);
	bool first_cycle = true;

	const double l = 0.5;
	double oldfx = 0, oldfy = 0;

	YarpPixelMono WHITE = 255;
	YarpPixelMono BLACK = 0;

	while (1)
	{
		in.Read();
		speed.Read();

		now = clock();
		last_cycle = ((now - m_prev_cycle) * 1000 / CLOCKS_PER_SEC);
		m_prev_cycle = now;

		in_image.Refer (in.Content());
		YARPColorConverter::RGB2Grayscale (in_image, mono);

		cur_position = speed.Content();

		of.Apply (mono, out_image, vx, vy);
		BuildHistogram (flow_histo, vx, vy, ox, oy);

		double jvx = (cur_position.j3 - old_position.j3) * 1000.0 / last_cycle;
		double jvy = (cur_position.j1 - old_position.j1) * 1000.0 / last_cycle;
		
		double jhx = (cur_position.j6 - old_position.j6) * 1000.0 / last_cycle;
		double jhy = (cur_position.j7 - old_position.j7) * 1000.0 / last_cycle;

		double roll = (cur_position.j5 - old_position.j5) * 1000.0 / last_cycle;
		double tilt = (cur_position.j4 - old_position.j4) * 1000.0 / last_cycle;

		jvx = l * jvx + (1-l) * oldfx;
		jvy = l * jvy + (1-l) * oldfy;

		old_position = cur_position;
		oldfx = jvx;
		oldfy = jvy;

		int * ptr = flow_histo.GetArrayPtr ()[0];

		const int width = flow_histo.GetWidth ();
		const int height = flow_histo.GetHeight ();

		int mx = 0, my = 0;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++, ptr++)
			{
				if (*ptr == flow_histo.GetMaximum())
				{
					mx = j - NCLUSTERS/2;
					my = i - NCLUSTERS/2;
				}
			}
		}

		const int VALUE = 0;
		const int SPAN = 45;

		bool necknotmoving = 
			(fabs(roll) < 500.0) &&
			(fabs(tilt) < 500.0) &&
			(fabs(jhx) < 6000.0) &&
			(fabs(jhy) < 6000.0);
		
		if ((jvy != 0.0 || jvx != 0.0) && 
			(fabs(double(mx)) < MAXFLOW && fabs(double(my)) < MAXFLOW) &&
			necknotmoving)
		{
			double angle = atan2 (2 * jvy, -jvx);
			double angle2 = 0;

			if (mx != 0 || my != 0)
			{
				angle2 = atan2 (double(my), double(mx));

				for (int t = -SPAN; t <= SPAN; t++)
				{
					for (int r = 0; r <= 8; r++)
					{
						double x = r * cos (angle2 + t * degToRad);
						double y = r * sin (angle2 + t * degToRad);
						if (fabs(x) <= MAXFLOW && fabs(y) <= MAXFLOW)
							flow_histo (x, y) = VALUE;
					}
				}
			}

			// mask the histogram.
			for (int t = -SPAN; t <= SPAN; t++)
			{
				for (int r = 0; r <= 8; r++)
				{
					double x = r * cos (angle + t * degToRad);
					double y = r * sin (angle + t * degToRad);
					if (fabs(x) <= MAXFLOW && fabs(y) <= MAXFLOW)
						flow_histo (x, y) = VALUE;
				}
			}

			BackprojectHistogram (hout, flow_histo, vx, vy, ox, oy);
		}
		else
		{
			for (int i = -1; i <= 1; i++)
				for (int j = -1; j <= 1; j++)
					flow_histo (double(i), double(j)) = VALUE;

				if (fabs(double(mx)) >= MAXFLOW || 
					fabs(double(my)) >= MAXFLOW ||
					!necknotmoving)
			{
				hout.Zero();
			}
			else
			{
				BackprojectHistogram (hout, flow_histo, vx, vy, ox, oy);
			}
		}

		//ConvertHistogram2Img (hout, flow_histo);

		Filter (in_image, grayfiltered);

		smooth.Apply (hout, hout);

		mono = hout;
		regiongrow.TagImage (&mono);
		regiongrow.DrawRegions (mono, vx, vy);

		YARPGenericImage& outImg = out.Content();
		outImg.Refer(out_image);
		out.Write();

		YARPGenericImage& outImg2 = phisto.Content();
		outImg2.Refer(mono);
		phisto.Write();

		YARPGenericImage& outImg3 = gray.Content();
		outImg3.Refer(grayfiltered);
		gray.Write();
	}
}
#endif


#if 0
		//
		double tx = 0, ty = 0;
		double avex = 0, avey = 0;
		bool valid = FindTarget (hout, BLOCKSIZE, &tx, &ty, 10, vx, vy, ox, oy, &avex, &avey);
		if (valid)
		{
			mono = hout;
			AddCircleOutline (mono, WHITE, int(tx), int(ty), 20);
			AddCircleOutline (out_image, BLACK, int(tx), int(ty), 20);
			AddSegment (mono, WHITE, int(tx), int(ty), int(tx+5*avex+.5), int(ty+5*avey+.5)); 
			AddSegment (out_image, BLACK, int(tx), int(ty), int(tx+5*avex+.5), int(ty+5*avey+.5)); 
		}
		else
			mono = hout;

		avel.Content().valid = valid;
		avel.Content().x = avex;
		avel.Content().y = avey;
		avel.Write();
#endif
