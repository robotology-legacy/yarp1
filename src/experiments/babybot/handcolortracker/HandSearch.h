#ifndef __YARPHANDSEGMENTATION__
#define __YARPHANDSEGMENTATION__

#ifndef NO_COMPILE
class YARPHandSegmentation
{
public:
	YARPHandSegmentation (double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char n):
	  histo(max, min, n)
	  {}
	  YARPHandSegmentation (double lumaTh, double satTh, unsigned char max, unsigned char min, unsigned char *n):
	  histo(max, min, n)
	  {}

	void search(YARPImageOf<YarpPixelHSV> &src, YARPImageOf<YarpPixelMono> &backpr, YARPImageOf<YarpPixelMono> &out, YARPLpHistoSegmentation &target, double R)
	{
		// perform complete search over the image
		// just plot image
		int r;
		int t;
		for(r = 0; r<_srho; r++)
			for(t = 0; t<_stheta; t++)
			{
				if ( backpr(t,r) > 250)
				{
					_fitter.findCircle(t, r, R, points);
					cumulateRegion(src, points);
					// now histo is the histogram of the current circle
					double p = intersect(target);
					out(t,r) = unsigned char (p*255 + 0.5);
				}
				else
					out(t,r) = 0;

			}
	}
		
	void cumulateRegion(YARPImageOf<YarpPixelHSV> &src, circle &points)
	{
		int m;
		histo.clean();
		for(m = 0; m < points.n; m++)
		{
			YarpPixelHSV pixel = src(points.t[m], points.r[m]);
			// later check weight
			histo.Apply(pixel.h, pixel.s, 0, 1);
		}
	}

	double intersect(YARPLpHistoSegmentation &target)
	{
		HistoEntry tmpG;
		HistoEntry tmpH;
			
		int it = 0;
		double sumG = 0.0;
		double sumH = 0.0;
		double sum = 0.0;
		while  ( (target.find(it, tmpG)!=-1) && (histo.find(it, tmpH)!=-1) )
		{
			double g = tmpG.value()/target.maximum();
			double h = tmpH.value()/histo.maximum();
			
			sum += (g-h)*(g-h);
			it++;
		}

		return sum;
	}

	circle points;
	YARP3DHistogram histo;
	YARPLpConicFitter _fitter;
};
#endif

#endif