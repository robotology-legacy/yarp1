// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include <YARPBlobDetector.h>

#include <YARPHistoSegmentation.h>

#include "HandKinematics.h"
#include "HandSegmenter.h"

using namespace _logpolarParams;

YARPLogpolar _mapper;

int main(int argc, char* argv[])
{
	YARPLpHistoSegmentation _histo(65, 50, 255, 0, 15);

	YARPBlobDetector _blobber((float) 0.0);
	_blobber.resize(_stheta, _srho, _sfovea);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _outPortBackprojection(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	YARPInputPortOf<YARPBottle>			_armSegmentationPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	HandKinematics _handLocalization;
	HandSegmenter _segmenter;
	
	_inPortImage.Register("/handtracker/i:img", "Net1");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_armSegmentationPort.Register("/handtracker/segmentation/i:armdata");
	_outPortBackprojection.Register("/handtracker/backprojection/o:img");
	
	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelMono> _leftSeg;
	YARPImageOf<YarpPixelBGR> _leftColored;
	YARPImageOf<YarpPixelBGR> _leftSegColored;
	YARPImageOf<YarpPixelHSV> _leftHSV;
	YARPImageOf<YarpPixelHSV> _leftSegHSV;
	_left.Resize(_stheta, _srho);
	_leftSeg.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);
	_leftSegColored.Resize(_stheta, _srho);
	_leftSegHSV.Resize(_stheta, _srho);
	_leftHSV.Resize(_stheta, _srho);

	YARPImageOf<YarpPixelMono> _outSeg;
	YARPImageOf<YarpPixelMono> _outSeg2;
	_outSeg.Resize(_stheta, _srho);
	_outSeg2.Resize(_stheta, _srho);

	char tmp[128];
	sprintf(tmp, "%s%d", "y:\\zgarbage\\exp19\\histo", 90);
	_histo.load(YARPString(tmp));

	YARPLogpolar _mapper;

	int _nHistos = 0;
	while (true)
	{
		_inPortImage.Read();
		
		if (_inPortSeg.Read(0))
		{
			ACE_OS::printf("Updating histogram\n");
			_leftSeg.Refer(_inPortSeg.Content());
			_mapper.ReconstructColor(_leftSeg, _leftSegColored);

			YARPColorConverter::RGB2HSV(_leftSegColored, _leftSegHSV);
			_histo.Apply(_leftSegHSV);

			// dump histo
			char tmp[128];
			_nHistos++;
			sprintf(tmp, "%s%d", "y:\\zgarbage\\histo", _nHistos);
			_histo.dump(YARPString(tmp));
		}

		if(_armSegmentationPort.Read(0))
		{
			_armSegmentationPort.Content().display();
			_handLocalization.learn(_armSegmentationPort.Content());
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);

		YARPColorConverter::RGB2HSV(_leftColored, _leftHSV);
		_histo.backProjection(_leftHSV, _outSeg);

		//_blobber.filterLp(_outSeg);
		YARPImageOf<YarpPixelMono> &tmp = _blobber.getSegmented();

		_segmenter.merge(tmp, _handLocalization.query());
		//_segmenter.search(_leftHSV, _histo, _handLocalization.query());

		_outPortBackprojection.Content().Refer(_outSeg);
		_outPortBackprojection.Write();
	}
	return 0;
}