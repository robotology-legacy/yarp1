#ifndef __HANDSEGMENTER__
#define __HANDSEGMENTER__

#include <YARPImage.h>
#include <YARPLogpolar.h>
#include <YARPPort.h>

const double __scale = 2.0;

class HandSegmenter
{
public:
	HandSegmenter():
	outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		outPort.Register("/handtracker/segmentation/o:img");
		tmpImage.Resize(_logpolarParams::_xsize/__scale, _logpolarParams::_ysize/__scale);
		outImage.Resize(_logpolarParams::_xsize, _logpolarParams::_ysize);
	}

	void merge(const YARPImageOf<YarpPixelMono> &in, const YARPShapeEllipse &el)
	{
		mapper.Logpolar2Cartesian(in, tmpImage);
		// decimate
		outImage.Zero();
		iplDecimate(tmpImage, outImage, 1, __scale, 1, __scale, IPL_INTER_LINEAR);
		int x,y;
		mapper.Logpolar2Cartesian(el.rho, el.theta, x, y);
		YARPSimpleOperation::DrawCross(outImage, x, y, YarpPixelRGB(255, 0, 0));
		_send();
	}
	
private:
	void _send()
	{
	
		outPort.Content().Refer(outImage);
		outPort.Write();
	}

	YARPOutputPortOf<YARPGenericImage>	outPort;
	YARPImageOf<YarpPixelRGB> tmpImage;
	YARPImageOf<YarpPixelRGB> outImage;
	
	YARPLogpolar mapper;
};

#endif