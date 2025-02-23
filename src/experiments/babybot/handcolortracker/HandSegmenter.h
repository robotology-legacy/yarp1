#ifndef __HANDSEGMENTER__
#define __HANDSEGMENTER__

#include <yarp/YARPImage.h>
#include <yarp/YARPLogpolar.h>
#include <yarp/YARPConicFitter.h>

const double __scale = 2.0;

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

		scale = __scale;
	}

	double mergeColor(YARPImageOf<YarpPixelBGR> &in, YARPImageOf<YarpPixelMono> &seg, YARPShapeEllipse el)
	{
		YARPLpShapeEllipse tmp;
		if (el.x > _logpolarParams::_xsize)
			el.x = _logpolarParams::_xsize;
		if (el.y > _logpolarParams::_ysize)
			el.y = _logpolarParams::_ysize;

		if (el.x < 0)
			el.x = 0;
		if (el.y < 0)
			el.y = 0;

		mapper.Cartesian2Logpolar((int) el.x, (int) el.y, (int) tmp.rho, (int) tmp.theta);
		tmp.a11 = el.a11; // 0.005;
		tmp.a12 = el.a12; // 0;
		tmp.a22 = el.a22; // 0.005;
	
		conicFitter.findEllipse(tmp, region);
		// conicFitter.plotEllipse(tmp, in);

		double v = _accumulate(seg, region);

		mapper.Logpolar2Cartesian(in, tmpImage);
		YARPSimpleOperation::Decimate(tmpImage, outImage, __scale, __scale);
		
		return v;
	}

	void mergeColor(YARPImageOf<YarpPixelBGR> &in, YARPShapeEllipse el)
	{
		YARPLpShapeEllipse tmp;
		if (el.x > _logpolarParams::_xsize)
			el.x = _logpolarParams::_xsize;
		if (el.y > _logpolarParams::_ysize)
			el.y = _logpolarParams::_ysize;

		if (el.x < 0)
			el.x = 0;
		if (el.y < 0)
			el.y = 0;

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
		drawCross(x, y, YarpPixelBGR(0, 255, 0), 10, 2);
	}

	inline void drawCross(int x, int y, const YarpPixelBGR &v, int h, int w)
	{
		#ifndef DO_NOT_DRAW
			YARPSimpleOperation::DrawCross(outImage, x, y, v, h, w);
		#endif
	}

	inline void plotEllipse(const YARPShapeEllipse &el, const YarpPixelBGR &v)
	{
		// #ifndef DO_NOT_DRAW
			conicFitterCart.plotEllipse(el, outImage, v);
		// #endif
	}
	
	inline void plotEllipse2(const YARPShapeEllipse &el, const YarpPixelBGR &v, YVector &fingers)
	{
		double delta = el.a11*el.a11+4*el.a12*el.a12-2*el.a11*el.a22+el.a22*el.a22;
		double m1 = (-el.a11+el.a22-sqrt(delta))/(2*el.a12);
		double m2 = (-el.a11+el.a22+sqrt(delta))/(2*el.a12);

		double x11 = -1/sqrt(el.a11+m1*(2*el.a12+el.a22*m1)); //el.x;
		double x12 = 1/sqrt(el.a11+m1*(2*el.a12+el.a22*m1)); //el.x;

		double x21 = -1/sqrt(el.a11+m2*(2*el.a12+el.a22*m2)); //+ el.x;
		double x22 = 1/sqrt(el.a11+m2*(2*el.a12+el.a22*m2)); // + el.x;

		printf("%.4lf\t%.4lf\t%.4lf\n", el.a11, el.a12, el.a22);

		double y11 = -m1*(x11); //el.y;
		double y12 = -m1*(x12); //el.y;
		double y21 = -m2*(x21); //el.y;
		double y22 = -m2*(x22); //el.y;

	//	printf("%.4lf\t%.4lf\t%.4lf\t%.4lf\n", x11, y11, x12, y12);
	//	printf("%.4lf\t%.4lf\t%.4lf\t%.4lf\n", x21, y21, x22, y22);
		
	#ifndef DO_NOT_DRAW
		YARPSimpleOperation::DrawCross(outImage, x11+el.x, y11+el.y, v, 3, 1);
		YARPSimpleOperation::DrawCross(outImage, x21+el.x, y21+el.y, v, 3, 1);
		YARPSimpleOperation::DrawCross(outImage, x12+el.x, y12+el.y, v, 3, 1);
		YARPSimpleOperation::DrawCross(outImage, x22+el.x, y22+el.y, v, 3, 1);
	#endif

		fingers(1) = x12+el.x;
		fingers(2) = y12+el.y;

				
		// conicFitterCart.plotEllipse(el, outImage, v);
	}

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
			// tmp += pSize(tmpT, tmpR)*value;
			tmp += mapper.Jacobian(tmpT, tmpR)*value;
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
	YARPConicFitter   conicFitterCart;
	YARPLpShapeRegion region;

	// histo intersection
	YARP3DHistogram histo;
};

#endif