#ifndef __HANDSEGMENTER__
#define __HANDSEGMENTER__

#include <YARPImage.h>
#include <YARPLogpolar.h>
#include <YARPPort.h>
#include <YARPConicFitter.h>

const double __scale = 4.0;

class HandSegmenter
{
public:
	HandSegmenter():
	outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP),
	histo(255, 0, 15)
	{
		outPort.Register("/handtracker/segmentation/o:img");
		tmpImage.Resize(_logpolarParams::_xsize, _logpolarParams::_ysize);
		outImage.Resize(_logpolarParams::_xsize/__scale, _logpolarParams::_ysize/__scale);
		tmpLp.Resize(_logpolarParams::_stheta, _logpolarParams::_srho);
		mask.Resize(_logpolarParams::_stheta, _logpolarParams::_srho);
	}

	void merge(const YARPImageOf<YarpPixelMono> &in, const YARPShapeEllipse &el)
	{
		mapper.ReconstructColor(mask, tmpLp);
		mapper.Logpolar2Cartesian(tmpLp, tmpImage);

		int x,y;
		mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		x = (x + _logpolarParams::_xsize/2)/__scale;
		y = (_logpolarParams::_ysize/2-y)/__scale;
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(255, 0, 0));
		_send();
	}

	void mergeColor(const YARPImageOf<YarpPixelBGR> &in, const YARPShapeEllipse &el)
	{
		mapper.Logpolar2Cartesian(in, tmpImage);
		YARPSimpleOperation::Decimate(tmpImage, outImage, __scale, __scale);
		int x,y;
		mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		x = (x + _logpolarParams::_xsize/2)/__scale;
		y = (_logpolarParams::_ysize/2-y)/__scale;
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(255, 0, 0));
		_send();
	}

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
		x = (x + _logpolarParams::_xsize/2)/__scale;
		y = (_logpolarParams::_ysize/2-y)/__scale;
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelBGR(v, 0, 0));
		//send 
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
	}
	
private:
	void _send()
	{
		outPort.Content().Refer(outImage);
		outPort.Write();
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
	
	void _createMask(const YARPShapeEllipse &el, YARPImageOf<YarpPixelMono> &mask)
	{
		mask.Zero();
		conicFitter.findEllipse(el, region);
		int i;
		for(i = 0; i < region.n; i++)
		{
			mask(region.t[i], region.r[i]) = 1;
		}
	}

	YARPOutputPortOf<YARPGenericImage>	outPort;
	YARPImageOf<YarpPixelBGR> tmpImage;
	YARPImageOf<YarpPixelMono> mask;
	YARPImageOf<YarpPixelBGR> outImage;
	YARPImageOf<YarpPixelBGR> tmpLp;
	
	YARPLogpolar mapper;
	YARPLpConicFitter conicFitter;
	YARPShapeRegion region;

	// histo intersection
	YARP3DHistogram histo;

};

#endif