//
// YARPConverter.cpp
//	- this file is compiled only under QNX.
//

#ifdef __QNX__

#include "YARPConverter.h"

//
//
// LATER: remove completely.

#if 0

//
void YARPConverter::ConvertFromCogToYARP (const GenericImage& src, YARPGenericImage& dst)
{
	GenericImage& tmp = (GenericImage&) src;

	assert (dst.GetPixelSize() == tmp.GetPixelSize());
	assert (dst.GetWidth() == tmp.GetWidth());
	assert (dst.GetHeight() == tmp.GetHeight());

	// it should be the same size.
	memcpy (dst.GetIplPointer()->imageData, tmp.GetRawBuffer(), dst.GetIplPointer()->imageSize);
}

void YARPConverter::ConvertFromYARPToCog (const YARPGenericImage& src, GenericImage& dst)
{
	assert (src.GetPixelSize() == dst.GetPixelSize());
	assert (src.GetWidth() == dst.GetWidth());
	assert (src.GetHeight() == dst.GetHeight());
	const int size = dst.GetWidth() * dst.GetHeight() * dst.GetPixelSize();

	memcpy (dst.GetRawBuffer(), src.GetIplPointer()->imageData, size);
}

void YARPConverter::ConvertFromYARPToCogRectangular (const YARPGenericImage& src, GenericImage& dst)
{
	assert (src.GetPixelSize() == dst.GetPixelSize());
	int pixsize = dst.GetPixelSize();

	char *bs = dst.GetRawBuffer ();

	IplImage *ipl = src.GetIplPointer ();
	const int dh = ipl->height;
	const int dw = ipl->width;
	char *ds = ipl->imageData;

	int depth = dst.GetPixelSize ();
	assert (depth == ipl->nChannels);	// same # of chan.

	const int h = dst.GetHeight();
	assert (h >= dh);			// same height.
    const int w = dst.GetWidth();
	assert (w >= dw);			// same width.

	const int rem_w = w - dw;

	for (int i = 0; i < dh; i++)
	{
		memcpy (bs, ds, dw * pixsize);

		bs += w * pixsize;
		ds += dw * pixsize;
	}
}

void YARPConverter::PasteInto (const YARPImageOf<YarpPixelMono>& src, int x, int y, int zoom, Image<PixelMono>& dst)
{
	char *bs = dst.GetRawBuffer ();

	IplImage *ipl = src.GetIplPointer ();
	const int dh = ipl->height;
	const int dw = ipl->width;
	char *dsY = ipl->imageData;

	int depth = dst.GetPixelSize ();
	assert (depth == ipl->nChannels);	// same # of chan.

	const int h = dst.GetHeight();
	assert (h >= dh);			// same height.
    const int w = dst.GetWidth();
	assert (w >= dw);			// same width.

	const int rem_w = w - dw;

	// crude limit check.
	assert (dw * zoom + x < w);
	assert (dh * zoom + y < h);

	if (zoom == 1)
	{
		bs += (y * w);
		for (int i = 0; i < dh; i++)
		{
			memcpy (bs + x, dsY, dw);

			bs += w;
			dsY += dw;
		}
	}
	else
	{
		bs += (y * w);
		for (int i = 0; i < dh; i++)
		{
			char * st_row = bs;
			bs += x;
			for (int j = 0; j < dw; j++)
			{
				for (int k = 0; k < zoom; k++)
				{
					*bs++ = *dsY;
				}
				dsY++;
			}

			for (int k = 1; k < zoom; k++)
				memcpy (st_row + x + w * k, st_row + x, dw * zoom); 

			bs = st_row + w * zoom;
		}
	}
}


// average across RF points.
// this converts directly from old image format to logpolar new format.
void YARPLpConverter::Cart2LpAverage(const GenericImage& is, YARPGenericImage& id) const
{
	GenericImage& tmp = (GenericImage&) is;

	assert (tmp.GetPixelSize() == id.GetPixelSize());
	assert(tmp.GetWidth() == Size && tmp.GetHeight() == Size);
	assert(id.GetWidth() == nEcc && id.GetHeight() == nAng);

	unsigned char **dst = (unsigned char **)id.GetArray ();
	unsigned char *src = (unsigned char *)tmp.GetRawBuffer ();

	switch (id.GetID())
	{
	default:
	case YARP_PIXEL_INVALID:
		printf ("*** LopPolar conversion: not a valid pixel type\n");
		exit(1);
		break;

	case YARP_PIXEL_MONO:
	case YARP_PIXEL_MONO_SIGNED:
		{
			int accumulator;

			for (int u = 0; u < nEcc; u++)
			{
				for (int v = 0; v < nAng; v++)
				{
					int index = v * nEcc + u;
					int rflinsize = x_ave[index][0];
					accumulator = 0;

					for (int kk = 0; kk < rflinsize; kk++)
					{
						accumulator += src[y_ave[index][kk] * Size + x_ave[index][kk+1]];
					}

					dst[v][u] = accumulator / rflinsize;
				}
			}
		}
		break;

	case YARP_PIXEL_RGB:
	case YARP_PIXEL_HSV:
	case YARP_PIXEL_BGR:
		{
			int accumulator0;
			int accumulator1;
			int accumulator2;

			for (int u = 0; u < nEcc; u++)
			{
				for (int v = 0; v < nAng; v++)
				{
					int index = v * nEcc + u;
					int rflinsize = x_ave[index][0];
					accumulator0 = 0;
					accumulator1 = 0;
					accumulator2 = 0;

					for (int kk = 0; kk < rflinsize; kk++)
					{
						int x = x_ave[index][kk+1];
						int y = y_ave[index][kk];
						int base = y * Size * 3 + x * 3;
						accumulator0 += src[base];
						accumulator1 += src[base+1];
						accumulator2 += src[base+2];
					}

					dst[v][u*3] = accumulator0 / rflinsize;
					dst[v][u*3+1] = accumulator1 / rflinsize;
					dst[v][u*3+2] = accumulator2 / rflinsize;
				}
			}
		}
		break;
	}
}

// swap RGB order. Sometimes the frame grabber version is BGR but it might
// be convenient for the processing to handle RGB instead.
void YARPLpConverter::Cart2LpAverageSwap(const Image<PixelBGR>& is, YARPImageOf<YarpPixelRGB>& id) const
{
	GenericImage& tmp = (GenericImage&) is;

	assert (tmp.GetPixelSize() == id.GetPixelSize());
	assert(tmp.GetWidth() == Size && tmp.GetHeight() == Size);
	assert(id.GetWidth() == nEcc && id.GetHeight() == nAng);

	unsigned char **dst = (unsigned char **)id.GetArray ();
	unsigned char *src = (unsigned char *)tmp.GetRawBuffer ();

	int accumulator0;
	int accumulator1;
	int accumulator2;

	for (int u = 0; u < nEcc; u++)
	{
		for (int v = 0; v < nAng; v++)
		{
			int index = v * nEcc + u;
			int rflinsize = x_ave[index][0];
			accumulator0 = 0;
			accumulator1 = 0;
			accumulator2 = 0;

			for (int kk = 0; kk < rflinsize; kk++)
			{
				int x = x_ave[index][kk+1];
				int y = y_ave[index][kk];
				int base = y * Size * 3 + x * 3;
				accumulator0 += src[base];
				accumulator1 += src[base+1];
				accumulator2 += src[base+2];
			}

			// accumulators are swapped to transform BRG->RGB.
			dst[v][u*3] = accumulator2 / rflinsize;
			dst[v][u*3+1] = accumulator1 / rflinsize;
			dst[v][u*3+2] = accumulator0 / rflinsize;
		}
	}
}

void YARPLpConverter::Lp2Cart(const YARPGenericImage& is, GenericImage& id) const
{
	assert (is.GetPixelSize() == id.GetPixelSize());
	assert(id.GetWidth() == Size && id.GetHeight() == Size);
	assert(is.GetWidth() == nEcc && is.GetHeight() == nAng);

	unsigned char** src = (unsigned char **)is.GetArray(); 
	unsigned char* dest = (unsigned char *)id.GetRawBuffer();

	switch (is.GetID())
	{
	default:
	case YARP_PIXEL_INVALID:
		printf ("*** LopPolar conversion: not a valid pixel type\n");
		exit(1);
		break;

	case YARP_PIXEL_MONO:
	case YARP_PIXEL_MONO_SIGNED:
		{
			for (int i=0;i<Size;i++)
				for (int j=0;j<Size;j++) 
				{
					if (*(l2cx+i+j*Size) == -1) 
						dest[j * Size + i] = 0;
					else
						dest[j * Size + i] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size)];
				}
		}
		break;

	case YARP_PIXEL_RGB:
	case YARP_PIXEL_HSV:
	case YARP_PIXEL_BGR:
		{
			for (int i=0;i<Size;i++)
				for (int j=0;j<Size;j++) 
				{
					if (*(l2cx+i+j*Size)==-1) 
					{
						int base = j * Size * 3 + i * 3;
						dest[base] = 0;
						dest[base+1] = 0;
						dest[base+2] = 0;
					}
					else
					{
						int base = j * Size * 3 + i * 3;
						dest[base] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3];
						dest[base+1] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3 + 1];
						dest[base+2] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3 + 2];
					}
				}
		}
		break;
	}
}

// swap channel order RGB->BGR.
void YARPLpConverter::Lp2CartSwap(const YARPImageOf<YarpPixelRGB>& is, Image<PixelBGR>& id) const
{
	assert (is.GetPixelSize() == id.GetPixelSize());
	assert(id.GetWidth() == Size && id.GetHeight() == Size);
	assert(is.GetWidth() == nEcc && is.GetHeight() == nAng);

	unsigned char** src = (unsigned char **)is.GetArray(); 
	unsigned char* dest = (unsigned char *)id.GetRawBuffer();

	for (int i=0;i<Size;i++)
		for (int j=0;j<Size;j++) 
		{
			if (*(l2cx+i+j*Size)==-1) 
			{
				int base = j * Size * 3 + i * 3;
				dest[base] = 0;
				dest[base+1] = 0;
				dest[base+2] = 0;
			}
			else
			{
				// channel order is changed.
				int base = j * Size * 3 + i * 3;
				dest[base+2] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3];
				dest[base+1] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3 + 1];
				dest[base] = src[*(l2cy+i+j*Size)][*(l2cx+i+j*Size) * 3 + 2];
			}
		}
}

#endif // closes #if 0.
//
//
//


void YARPBoxCopier::Copy (const YARPBox& src, CBox2Send& dst)
{
	dst.valid = src.valid;
	dst.rmax = src. rmax;
	dst.cmax = src.cmax;
	dst.rmin = src.rmin;
	dst.cmin = src.cmin;

	dst.xmax = src.xmax;
	dst.xmin = src.xmin;
	dst.ymax = src.ymax;
	dst.ymin = src.ymin;

	dst.total_sal = src.total_sal;
	dst.total_pixels = src.total_pixels;

	dst.xsum = src.xsum;
	dst.ysum = src.ysum;

	dst.centroid_x = src.centroid_x;
	dst.centroid_y = src.centroid_y;
}


#endif	// __QNX__