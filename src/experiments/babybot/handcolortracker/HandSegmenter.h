#ifndef __HANDSEGMENTER__
#define __HANDSEGMENTER__

#include <YARPImage.h>
#include <YARPLogpolar.h>
#include <YARPConicFitter.h>

const double __scale = 2.0;
const double __threshold = 150000;

class HandSegmenter
{
public:
	HandSegmenter():
	histo(255, 0, 15)
	{
		tmpImage.Resize(_logpolarParams::_xsize, _logpolarParams::_ysize);
		outImage.Resize(_logpolarParams::_xsize/__scale, _logpolarParams::_ysize/__scale);
		tmpLp.Resize(_logpolarParams::_stheta, _logpolarParams::_srho);
		mask.Resize(_logpolarParams::_stheta, _logpolarParams::_srho);

		threshold = __threshold;
		scale = __scale;
	}

	void mergeColor(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &seg, const YARPShapeEllipse &el)
	{
		YARPLpShapeEllipse tmp;
		mapper.Cartesian2Logpolar((int) el.x, (int) el.y, (int) tmp.rho, (int) tmp.theta);
		tmp.a11 = el.a11; // 0.005;
		tmp.a12 = el.a12; // 0;
		tmp.a22 = el.a22; // 0.005;
	
		conicFitter.findEllipse(tmp, region);
		conicFitter.plotEllipse(tmp, in);

		double v = _accumulate(seg, region);

		mapper.Logpolar2Cartesian(in, tmpImage);
		YARPSimpleOperation::Decimate(tmpImage, outImage, __scale, __scale);
		int x,y;
		x = el.x/__scale;
		y = el.y/__scale;
		// mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		// x = (x + _logpolarParams::_xsize/2)/__scale;
		// y = (_logpolarParams::_ysize/2-y)/__scale;
		if (v>threshold)
			YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(255, 0, 0), 5, 1);
		else
			YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(150, 0, 0), 5, 1);
	}

	void mergeColor(YARPImageOf<YarpPixelBGR> &in, const YARPShapeEllipse &el)
	{
		YARPLpShapeEllipse tmp;
		mapper.Cartesian2Logpolar((int) el.x, (int) el.y, (int) tmp.rho, (int) tmp.theta);
		tmp.a11 = el.a11;
		tmp.a12 = el.a12;
		tmp.a22 = el.a22;
		
		conicFitter.plotEllipse(tmp, in);
		mapper.Logpolar2Cartesian(in, tmpImage);
		YARPSimpleOperation::Decimate(tmpImage, outImage, __scale, __scale);
		int x,y;
		x = el.x/__scale;
		y = el.y/__scale;
		// mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		// x = (x + _logpolarParams::_xsize/2)/__scale;
		// y = (_logpolarParams::_ysize/2-y)/__scale;
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(0, 255, 0), 10, 2);
	}

/*
	void search(YARPImageOf<YarpPixelHSV> &src, YARPLpHistoSegmentation &target, const YARPShapeEllipse &el)
	{
		conicFitter.findEllipse(el, region);
		cumulateRegion(src, region);
		cumulateRegion(src, region);
		// now histo is the histogram of the current circle
		double p = intersect(target);
		unsigned char v = p*255;
		if (v>255)
			v = 255;
		
		outImage.Zero();
		int x,y;
		mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		///x = (x + _logpolarParams::_xsize/2)/__scale;
		///y = (_logpolarParams::_ysize/2-y)/__scale;
		x /= __scale;
		y /= __scale;
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(v, 0, 0));
		_send();
	}
		
	void cumulateRegion(YARPImageOf<YarpPixelHSV> &src, YARPShapeRegion &points)
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
	}*/
	
	YARPImageOf<YarpPixelBGR> &getImage()
	{
		return outImage;
	}

	double scale;

private:
	double _accumulate(YARPImageOf<YarpPixelMono> &in, YARPLpShapeRegion &region)
	{
		int i;
		double tmp = 0.0;
		for(i = 0; i < region.n; i++)
		{
			int tmpT = region.t[i];
			int tmpR = region.r[i];

			int value = in(tmpT, tmpR);
			tmp += pSize(tmpT, tmpR)*value;
		}

		return tmp;
	}

	void _filter(const YARPImageOf<YarpPixelMono> &in, YARPImageOf<YarpPixelMono> &out)
	{
		int i;
		int j;
		for (i = 0; i < _logpolarParams::_srho; i++)
		{
			char *tmpIn = in.GetArray()[i];
			char *tmpOut = out.GetArray()[i];
			for(j = 0; j < _logpolarParams::_stheta; j++)
			{
				(*tmpOut) = (*tmpOut)*(*tmpIn);
				// (*tmpOut) = (*tmpIn)*10;
				if ( (*tmpOut) >255)
					*tmpOut = (char) 255;

				tmpOut++;
				tmpIn++;
			}
		}
	}
	
/*	void _createMask(const YARPShapeEllipse &el, YARPImageOf<YarpPixelMono> &mask)
	{
		mask.Zero();
		conicFitter.findEllipse(el, region);
		int i;
		for(i = 0; i < region.n; i++)
		{
			mask(region.t[i], region.r[i]) = 1;
		}
	}*/

	YARPImageOf<YarpPixelBGR> tmpImage;
	YARPImageOf<YarpPixelMono> mask;
	YARPImageOf<YarpPixelBGR> outImage;
	YARPImageOf<YarpPixelBGR> tmpLp;
	
	YARPLogpolar mapper;
	YARPLpConicFitter conicFitter;
	YARPLpShapeRegion region;

	// histo intersection
	YARP3DHistogram histo;

	double threshold;
};

#endif