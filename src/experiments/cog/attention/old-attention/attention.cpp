//
// attention.cpp
//	- June 2001.
//
#include <sys/kernel.h>
#include <iostream.h>
#include <unistd.h>

// Remember to include the new version first!
#include <VisDMatrix.h>

#include "YARPImageServices.h"
#include "YARPPort.h"
#include "YARPImagePort.h"
#include "YARPTime.h"

#include <conf/tx_data.h>
#include <conf/lp_layout.h>

const int NUMBEROFCHANNELS = 9;
const int NUMBEROFBOXES = 5;
const int MAXBOXES = NUMBEROFCHANNELS * NUMBEROFBOXES;

YARPInputPortOf<FiveBoxesInARow> channels[NUMBEROFCHANNELS];
char portnames[NUMBEROFCHANNELS][256];

// relative weight for channels.
// the only difference among channels is the weight.
const int relative_weight[NUMBEROFCHANNELS] = 
{ 
	1, 1, 2, 2,	3, 3, 3, 3, 15
};

YARPOutputPortOf<StereoPosData> out;
YARPOutputPortOf<YARPGenericImage> out_saliency;

char *my_name = "/yarp/attention";

char out_pos_data_name[256];
char out_saliency_name[256];

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

	for (i = 0; i < NUMBEROFCHANNELS; i++)
	{
		sprintf (portnames[i], "%s/in.%d.%d", my_name, i, relative_weight[i]);
	}

	sprintf (out_pos_data_name, "%s/out", my_name); 
	sprintf (out_saliency_name, "%s/out/saliency", my_name);
}

inline int Area (CBox2Send *t)
{
	return (t->xmax - t->xmin) * (t->ymax - t->ymin);
}


void DrawBoxes (FiveBoxesInARow boxes, YARPImageOf<YarpPixelMono>& img, int weight, int scalefactor)
{
	char *ptri = img.GetRawBuffer ();
	const int w = img.GetWidth ();
	const int h = img.GetHeight ();

	// weird stuff...
	//
	CBox2Send *array = &(boxes.box1);

	for (int i = 0; i < 5; i++)
	{
		CBox2Send *tmp = array++;
		if (tmp->valid && Area (tmp) > 500)
		{
			// draw box i-th
			int minr = (tmp->ymin < 0) ? 0 : tmp->ymin;
			int maxr = (tmp->ymax >= h) ? h-1 : tmp->ymax;
			int minc = (tmp->xmin < 0) ? 0 : tmp->xmin;
			int maxc = (tmp->xmax >= w) ? w-1 : tmp->xmax;

			for (int r = minr; r <= maxr; r++)
			{
				char *row = ptri + r * w + tmp->xmin;
				for (int c = minc; c <= maxc; c++)
				{
					int result = *row + weight * scalefactor;
					if (result >= 255)
						*row++ = 255;
					else
						*row++ = result;
				}
			}
		}
	}
}

void FindCentroid (YARPImageOf<YarpPixelMono>& img, int *x, int *y)
{
	char *ptri = img.GetRawBuffer ();
	const int w = img.GetWidth ();
	const int h = img.GetHeight ();

	unsigned char max = 0;

	unsigned char *tmp = (unsigned char *)ptri;
	for (int i = 0; i < w * h; i++, tmp++)
		if (*tmp > max)
		{
			max = *tmp;
		}

	int count = 0;
	*x = 0;
	*y = 0;

	for (i = 0; i < h; i++)
	{
		unsigned char *row = (unsigned char *)ptri + i * w;
		for (int j = 0; j < w; j++, row++)
		{
			if (*row == max)
			{
				*x += j;
				*y += i;
				count ++;
			}
		}
	}

	if (count != 0)
	{
		*x /= count;
		*y /= count;
	}
	else
	{
		*x = img.GetWidth() / 2;
		*y = img.GetHeight() / 2;
	}
}

void TmpFilter (YARPImageOf<YarpPixelMono>& old, YARPImageOf<YarpPixelMono>& img)
{
	char *src = img.GetRawBuffer();
	char *optr = old.GetRawBuffer();
	const int w = img.GetWidth ();
	const int h = img.GetHeight ();

	for (int i = 0; i < w * h; i++)
	{
		*optr = 0.5 * *src++ + 0.5 * *optr;
		optr++;
	}

	memcpy (img.GetRawBuffer(), old.GetRawBuffer(), w * h);
}

void main(int argc, char *argv[])
{
	ParseCommandLine (argc, argv);

	StereoPosData out_data;
	
	YARPImageOf<YarpPixelMono> out_image;
	YARPImageOf<YarpPixelMono> old_image;
	out_image.Resize (Size, Size);
	old_image.Resize (Size, Size);
	memset (old_image.GetRawBuffer(), 0, Size * Size);

	// register names.
	for (int i = 0; i < NUMBEROFCHANNELS; i++)
	{
		channels[i].Register (portnames[i]);
	}

	out_saliency.Register (out_saliency_name);
	out.Register (out_pos_data_name);
	
	FiveBoxesInARow boxes[NUMBEROFCHANNELS];

	for (i = 0; i < NUMBEROFCHANNELS; i++)
	{
		memset (boxes+i, 0, sizeof(FiveBoxesInARow));
		boxes[i].box1.valid = false;
		boxes[i].box2.valid = false;
		boxes[i].box3.valid = false;
		boxes[i].box4.valid = false;
		boxes[i].box5.valid = false;
	}

	// compute saliency scale factor.
	int scalefactor = 0;
	for (i = 0; i < NUMBEROFCHANNELS; i++)
		scalefactor += relative_weight[i];

	scalefactor = 255 / scalefactor;
	if (scalefactor == 0)
		scalefactor = 1;

	YARPTime::DelayInSeconds(1.0);

	while (1)
	{
		// read all the valid boxes and try to fuse them...
		for (int i = 0; i < NUMBEROFCHANNELS; i++)
		{
			if (channels[i].Read(0))
			{
				boxes[i] = channels[i].Content();
			}
		}

		out_image.Zero ();

		// add channel boxes (5) to out_image.
		for (i = 0; i < NUMBEROFCHANNELS; i++)
			DrawBoxes (boxes[i], out_image, relative_weight[i], scalefactor);

		//TmpFilter (old_image, out_image);

		int xx = 0, yy = 0;
		FindCentroid (out_image, &xx, &yy);

		out_data.valid = true;
		out_data.xl = xx;
		out_data.yl = yy;
		out_data.xr = xx;
		out_data.yr = yy;

		YARPGenericImage& outImg = out_saliency.Content(); // Lasts until next Write()
		outImg.PeerCopy(out_image);
		out_saliency.Write();					

		out.Content() = out_data;
		out.Write();
		delay(30);
	}
}

