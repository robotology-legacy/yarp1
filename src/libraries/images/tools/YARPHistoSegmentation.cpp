#include "YARPHistoSegmentation.h"
#include "YARPLogpolar.h"

YARPHistoSegmentation::YARPHistoSegmentation(double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char n):
 YARP3DHistogram(max, min, n)
{
	_lumaThreshold = lumaTh;
	_satThreshold = satTh;
}

YARPHistoSegmentation::YARPHistoSegmentation(double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char *n):
YARP3DHistogram(max, min, n)
{
	_lumaThreshold = lumaTh;
	_satThreshold = satTh;
}

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelBGR> &src)
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

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelRGB> &src)
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

void YARPHistoSegmentation::Apply(YARPImageOf<YarpPixelHSV> &src)
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

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelRGB> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelRGB *src;
	YarpPixelRGB tmp;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelRGB *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			_normalize(*src, tmp);
			*dst = YARP3DHistogram::backProjection(tmp)*255 + 0.5;
			src++;
			dst++;
		}
	}
}

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelBGR *src;
	YarpPixelRGB tmp;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelBGR *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];
		for(i = 0; i < in.GetWidth(); i++)
		{
			_normalize(*src, tmp);
			*dst = YARP3DHistogram::backProjection(tmp)*255 + 0.5;
			src++;
			dst++;
		}
	}
}

void YARPHistoSegmentation::backProjection(YARPImageOf<YarpPixelHSV> &in, YARPImageOf<YarpPixelMono> &out)
{	
	// complete histogram backprojection
	int i;
	int j;
	YarpPixelHSV *src;
	YarpPixelMono *dst;
	for(j = 0; j < in.GetHeight(); j++)
	{
		src = (YarpPixelHSV *) in.GetArray()[j];
		dst = (YarpPixelMono *) out.GetArray()[j];

		for(i = 0; i < in.GetWidth(); i++)
		{
			if (_checkThresholds(src->h, src->s, src->v))
				*dst = YARP3DHistogram::backProjection(src->h, src->s, 0)*255 + 0.5;
			else
				*dst = 0;
			
			src++;
			dst++;
		}
	}
}

//
// logpolar class differs only in the apply methods
void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelBGR> &src)
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
		double w = pSize(1, i);
		for(j = 0; j < src.GetWidth(); j++)
		{
			YARPHistoSegmentation::_normalize(*r, *g, *b, &rp, &gp, &bp);
			YARP3DHistogram::Apply(rp, gp, bp, w);
			b += 3;
			g += 3;
			r += 3;
		}
	}

}

void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelRGB> &src)
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
		double w = pSize(1, i);	
		for(j = 0; j < src.GetWidth(); j++)
		{
			YARPHistoSegmentation::_normalize(*r, *g, *b, &rp, &gp, &bp);
			YARP3DHistogram::Apply(rp, gp, bp, w);
				
			b += 3;
			g += 3;
			r += 3;
		}
	}
}

void YARPLpHistoSegmentation::Apply(YARPImageOf<YarpPixelHSV> &src)
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

		double w = pSize(1, i);
		for(j = 0; j < src.GetWidth(); j++)
		{
			if (_checkThresholds(*h,*s,*v))
				YARP3DHistogram::Apply(*h, *s, 0, w);
						
			h += 3;
			s += 3;
			v += 3;
		}
	}
}