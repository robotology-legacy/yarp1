// HandColorTracker.cpp : Defines the entry point for the console application.
//

#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include <YARPVectorPortContent.h>
#include <YARPPort.h>

#include <YARPBlobDetector.h>

#include <YARPHistoSegmentation.h>

#include "HandKinematics.h"
#include "HandSegmenter.h"

using namespace _logpolarParams;

YARPLogpolar _mapper;

int __armCounter = 0;
int __headCounter = 0;

inline bool pollPort(YARPInputPortOf<YVector> &port, YVector &out, int *counter)
{
	bool ret = false;
	if (port.Read(0) )
	{
		out = port.Content();
		ret = true;
		(*counter)++;
	}

	return ret;
}

inline bool pollPort(YARPInputPortOf<YARPControlBoardNetworkData> &port, YVector &out, int *counter)
{
	bool ret = false;
	if (port.Read(0) )
	{
		out = port.Content()._current_position;
		ret = true;
		(*counter)++;
	}

	return ret;
}

inline bool pollPort(YARPInputPortOf<YARPGenericImage> &port, YARPGenericImage &out)
{
	bool ret = false;
	if (port.Read(0))
	{
		out.Refer(port.Content());
		ret = true;
	}

	return ret;
}

void printFrameStatus(int n);

int main(int argc, char* argv[])
{
	unsigned char bins[] = {16, 16, 1};
	YARPLpHistoSegmentation _histo(80, 80, 255, 0, bins);

	YARPBlobDetector _blobber((float) 0.0);
	_blobber.resize(_stheta, _srho, _sfovea);

	YARPInputPortOf<YARPGenericImage> _inPortImage(YARPInputPort::DEFAULT_BUFFERS, YARP_MCAST);
	YARPInputPortOf<YARPGenericImage> _inPortSeg(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YARPGenericImage> _outPortBackprojection(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	YARPInputPortOf<YARPControlBoardNetworkData> _armPort;
	YARPInputPortOf<YVector>	_headPort;

	YARPInputPortOf<YARPBottle>	_armSegmentationPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);

	YARPInputPortOf<YARPBottle>	_outputPortTrain1(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
	YARPInputPortOf<YARPBottle>	_inputPortTrain1(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPInputPortOf<YARPBottle>	_outputPortTrain2(YARPOutputPort::DEFAULT_OUTPUTS, YARP_TCP);
	YARPInputPortOf<YARPBottle>	_inputPortTrain2(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);

	YARPBottle _outputTrainBottle1;
	YARPBottle _outputTrainBottle2;

	HandKinematics _handLocalization;
	HandSegmenter _segmenter;
	
	_inPortImage.Register("/handtracker/i:img", "Net1");
	_inPortSeg.Register("/handtracker/segmentation/i:img");
	_armSegmentationPort.Register("/handtracker/segmentation/i:armdata");
	_outPortBackprojection.Register("/handtracker/backprojection/o:img");
	_headPort.Register("/handtracker/head/i");
	_armPort.Register("/handtracker/arm/i");
	_inputPortTrain1.Register("/handtracker/nnet1/i");
	_outputPortTrain1.Register("/handtracker/nnet1/o");
	_inputPortTrain2.Register("/handtracker/nnet2/i");
	_outputPortTrain2.Register("/handtracker/nnet2/o");
	
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

	YVector _arm(6);
	YVector _head(5);

	YARPImageOf<YarpPixelMono> _outSeg;
	YARPImageOf<YarpPixelMono> _outSeg2;
	_outSeg.Resize(_stheta, _srho);
	_outSeg2.Resize(_stheta, _srho);

	char tmp[128];
	sprintf(tmp, "%s%d", "y:\\zgarbage\\exp20\\histo", 59);
	_histo.load(YARPString(tmp));

	YARPLogpolar _mapper;

	int _nHistos = 0;
	int _nFrames = 0;
	while (true)
	{
		// blocking on ports
		_inPortImage.Read();

		/////////////////////////// segmentation
		// poll segmentation
		if (pollPort(_inPortSeg, _leftSeg))
		{
			ACE_OS::printf("Updating histogram\n");
			_mapper.ReconstructColor(_leftSeg, _leftSegColored);
			YARPColorConverter::RGB2HSV(_leftSegColored, _leftSegHSV);
			_histo.Apply(_leftSegHSV);
			// dump histo
			char tmp[128];
			_nHistos++;
			sprintf(tmp, "%s%d", "y:\\zgarbage\\histo", _nHistos);
			_histo.dump(YARPString(tmp));
		}
		///////////////////////////////////////////

		///// poll head and arm positions
		pollPort(_armPort, _arm, &__armCounter);
		pollPort(_headPort, _head, &__headCounter);

		if(_armSegmentationPort.Read(0))
		{
			_armSegmentationPort.Content().display();
			_outputTrainBottle1.reset();
			_outputTrainBottle2.reset();
			// _handLocalization.learn(_arm, _head,  _armSegmentationPort.Content());
			prepareBottle1(_armSegmentation.Content(), _outputTrainBottle1);
			prepareBottle2(_armSegmentation.Content(), _outputTrainBottle2);
		}

		
		_left.Refer (_inPortImage.Content());
		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);
		YARPColorConverter::RGB2HSV(_leftColored, _leftHSV);
		_histo.backProjection(_leftHSV, _outSeg);

		// _blobber.filterLp(_outSeg);
		// YARPImageOf<YarpPixelMono> &tmp = _blobber.getSegmented();

		// _segmenter.merge(tmp, _handLocalization.query());
		// _segmenter.merge(_outSeg, _handLocalization.query());
		_segmenter.mergeColor(_leftColored, _handLocalization.query(_arm, _head));
		// _segmenter.merge(_outSeg, _handLocalization.query());
		//_segmenter.search(_leftHSV, _histo, _handLocalization.query());

		_outPortBackprojection.Content().Refer(_outSeg);
		_outPortBackprojection.Write();

		if (_inputPortTrain1.Read(0))
		{
			ACE_OS::printf("Loading new trained nnet1");
			YARPBPNNetState tmp;
			extract(_inputPortTrain1.Content(), tmp);
			_handLocalization.loadCenter(tmp);
			ACE_OS::printf("...done!\n");
		}

		if (_inputPortTrain2.Read(0))
		{
			ACE_OS::printf("Loading new trained nnet2");
			YARPBPNNetState tmp;
			extract(_inputPortTrain2.Content(), tmp);
			_handLocalization.loadEllipse(tmp);
			ACE_OS::printf("...done!\n");
		}

		printFrameStatus(_nFrames);
		_nFrames++;
	}
	return 0;
}


////////// print frame status
void printFrameStatus(int n)
{
	if (n%100 == 0)
		ACE_OS::printf("HeadFrame# %d\tArmFrame# %d\n", __headCounter, __armCounter);
}

