#ifndef __YARPHISTOSEGMENTATIONHH__
#define __YARPHISTOSEGMENTATIONHH__

#include "YARP3DHistogram.h"

class YARPHistoSegmentation: public YARP3DHistogram
{
public:
	YARPHistoSegmentation(double lumaTh, unsigned char max, unsigned char min, unsigned char n):
	  YARP3DHistogram(max, min, n)
	{
		_lumaThreshold = lumaTh;
	
	}

	YARPHistoSegmentation(double lumaTh, unsigned char max, unsigned char min, unsigned char *n):
	  YARP3DHistogram(max, min, n)
	{
		_lumaThreshold = lumaTh;
	}
	
	inline void _normalize (unsigned char r, unsigned char g, unsigned char b,
							unsigned char *rp, unsigned char *gp, unsigned char *bp)
	{
		*rp = r;
		*gp = g;
		*bp = b;

		float luma = r+g+b;
		if (luma < _lumaThreshold)
		{
			*rp = 0;
			*gp = 0;
			*bp = 0;
		}
		else
		{
			*rp = (r/luma)*255 + 0.5;
			*gp = (g/luma)*255 + 0.5;
			*bp = (b/luma)*255 + 0.5;
		}
	}
	
	inline void _normalize (const YarpPixelRGB &in, YarpPixelRGB &out)
	{
		_normalize(in.r, in.g, in.b, &out.r, &out.g, &out.b);
	}

	inline void _normalize (const YarpPixelBGR &in, YarpPixelRGB &out)
	{
		_normalize(in.r, in.g, in.b, &out.r, &out.g, &out.b);
	}

	void Apply(YARPImageOf<YarpPixelBGR> &src)
	{
		int i;
		int j;
		unsigned char *r;
		unsigned char *g;
		unsigned char *b;

		for(i = 0; i < src.GetHeight(); i++)
		{
			b = (unsigned char *) src.GetArray()[i];
			g = b+1;
 			r = b+2;

			unsigned char rp, gp, bp;
		
			
			for(j = 0; j < src.GetWidth(); j++)
			{
				_normalize(*r, *g, *b, &rp, &gp, &bp);
				YARP3DHistogram::Apply(rp, gp, bp);
				b += 3;
				g += 3;
				r += 3;
			}
		}

	}

	void Apply(YARPImageOf<YarpPixelRGB> &src)
	{
		int i;
		int j;
		unsigned char *r;
		unsigned char *g;
		unsigned char *b;
			
		for(i = 0; i < src.GetHeight(); i++)
		{
			r = (unsigned char *) src.GetArray()[i];
			g = r+1;
 			b = r+2;

			unsigned char rp, gp, bp;
					
			for(j = 0; j < src.GetWidth(); j++)
			{
				_normalize(*r, *g, *b, &rp, &gp, &bp);
				YARP3DHistogram::Apply(rp, gp, bp);
				
				b += 3;
				g += 3;
				r += 3;
			}
		}
	}

	void Apply(YARPImageOf<YarpPixelHSV> &src)
	{
		int i;
		int j;
		unsigned char *h;
		unsigned char *s;
		unsigned char *v;
			
		for(i = 0; i < src.GetHeight(); i++)
		{
			h = (unsigned char *) src.GetArray()[i];
			s = h+1;
 			v = h+2;

			for(j = 0; j < src.GetWidth(); j++)
			{
				YARP3DHistogram::Apply(*h, *s, 0);
				
				h += 3;
				s += 3;
				v += 3;
			}
		}
	}

	void backProjection(YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out)
	{	
		// complete histogram backprojection
		int i;
		int j;
		for(j = 0; j < in.GetHeight(); j++)
			for(i = 0; i < in.GetWidth(); i++)
			{
				YarpPixelRGB tmp;
				_normalize(in(i,j), tmp);
				out(i,j) = YARP3DHistogram::backProjection(tmp)*255 + 0.5;
			}
	}

	void backProjection(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
	{	
		// complete histogram backprojection
		int i;
		int j;
		for(j = 0; j < in.GetHeight(); j++)
			for(i = 0; i < in.GetWidth(); i++)
			{
				YarpPixelRGB tmp;
				_normalize(in(i,j), tmp);
				out(i,j) = YARP3DHistogram::backProjection(tmp)*255 + 0.5;
			}
	}

	void backProjection(YARPImageOf<YarpPixelHSV> &in, YARPImageOf<YarpPixelMono> &out)
	{	
		// complete histogram backprojection
		int i;
		int j;
		for(j = 0; j < in.GetHeight(); j++)
			for(i = 0; i < in.GetWidth(); i++)
			{
				YarpPixelHSV tmp;
				tmp = in(i,j);
				out(i,j) = YARP3DHistogram::backProjection(tmp.h, tmp.s, 0)*255 + 0.5;
			}
	}


private:
	double _lumaThreshold;
};

#endif
