// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include "YARPHistoSegmentation.h"
#include "YARP3DHistogram.h"

using namespace _logpolarParams;

int main(int argc, char* argv[])
{
	YARPHistoSegmentation _histo(5, 255, 0, 10);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);

	YARPOutputPortOf<YARPGenericImage> _outPortSeg(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	YARPBlobDetector _blobber(5.0);
	_blobber.resize(_stheta, _srho, _sfovea);
	
	_inPortImage.Register("/handtracker/i:img");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_outPortSeg.Register("/handtracker/segmentation/o:img");

	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelMono> _leftSeg;
	YARPImageOf<YarpPixelBGR> _leftColored;
	YARPImageOf<YarpPixelBGR> _leftSegColored;
	YARPImageOf<YarpPixelHSV> _leftHSV;
	_left.Resize(_stheta, _srho);
	_leftSeg.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);
	_leftSegColored.Resize(_stheta, _srho);
	_leftHSV.Resize(_stheta, _srho);

	YARPImageOf<YarpPixelMono> _outSeg;
	// YARPImageOf<YarpPixelMono> _blobs;
	_outSeg.Resize(_stheta, _srho);
	// _blobs.Resize(_stheta, _srho);

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

			YARPColorConverter::RGB2HSV(_leftSegColored, _leftHSV);
			_histo.Apply(_leftHSV);

			// dump histo
			char tmp[128];
			_nHistos++;
			sprintf(tmp, "%s%d", "y:\\zgarbage\\histo", _nHistos);
			_histo.dump(YARPString(tmp));
		}

		_left.Refer (_inPortImage.Content());

		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);

		_histo.backProjection(_leftColored, _outSeg);
				
		_outPortSeg.Content().Refer(_outSeg);
		_outPortSeg.Write();

		/*_nHistos++;
		if ((_frame%100)==0)
			ACE_OS::printf("frame #%5d\r", _nHistos);*/
	}
	return 0;
}

