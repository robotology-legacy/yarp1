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

const char *__nnetFile1 = "handfk1.ini";
const char *__nnetFile2 = "handfk2.ini";

int __armCounter = 0;
int __headCounter = 0;

#define DECLARE_INPUT_PORT(TYPE, NAME, PROTOCOL) YARPInputPortOf<TYPE> NAME(YARPInputPort::DEFAULT_BUFFERS, PROTOCOL)
#define DECLARE_OUTPUT_PORT(TYPE, NAME, PROTOCOL) YARPOutputPortOf<TYPE> NAME(YARPOutputPort::DEFAULT_OUTPUTS, PROTOCOL)


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

void dumpError(YARPShapeEllipse &ellipse, YARPBottle &segData);

YARPLogFile _log;

int main(int argc, char* argv[])
{
	unsigned char bins[] = {16, 16, 1};
	YARPLpHistoSegmentation _histo(80, 80, 255, 0, bins);

	YARPBlobDetector _blobber((float) 0.0);
	_blobber.resize(_stheta, _srho, _sfovea);

	DECLARE_INPUT_PORT (YARPGenericImage, _inPortImage, YARP_MCAST);
	DECLARE_INPUT_PORT (YARPGenericImage, _inPortSeg, YARP_UDP);
	DECLARE_OUTPUT_PORT (YARPGenericImage, _outPortBackprojection, YARP_UDP);
	DECLARE_OUTPUT_PORT (YARPGenericImage, _outPortColor, YARP_UDP);

	DECLARE_OUTPUT_PORT (YVector, _outputPortHandPosition, YARP_UDP);
	DECLARE_OUTPUT_PORT (YVector, _outputPortHandPredicted, YARP_UDP);
	
	DECLARE_INPUT_PORT (YARPControlBoardNetworkData, _armPort, YARP_UDP);
	DECLARE_INPUT_PORT (YVector, _headPort, YARP_UDP);

	DECLARE_INPUT_PORT (YARPBottle,	_armSegmentationPort, YARP_TCP);
	
	DECLARE_OUTPUT_PORT (YARPBottle, _outputPortTrain1, YARP_TCP);
	DECLARE_INPUT_PORT (YARPBottle, _inputPortTrain1, YARP_TCP);
	DECLARE_OUTPUT_PORT (YARPBottle, _outputPortTrain2, YARP_TCP);
	DECLARE_INPUT_PORT (YARPBottle, _inputPortTrain2, YARP_TCP);
	
	YARPBottle _outputTrainBottle1;
	YARPBottle _outputTrainBottle2;

	HandKinematics _handLocalization(__nnetFile1, __nnetFile2);
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
	_outputPortHandPosition.Register("/handtracker/position/o");
	_outputPortHandPredicted.Register("/handtracker/predicted/o");
	_outPortColor.Register("/handtracker/segmentation/o:img");
	
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
	YVector _handPosition(2);

	YARPImageOf<YarpPixelMono> _outSeg;
	YARPImageOf<YarpPixelMono> _outSeg2;
	_outSeg.Resize(_stheta, _srho);
	_outSeg2.Resize(_stheta, _srho);

	char tmp[128];
	sprintf(tmp, "%s%d", "y:\\zgarbage\\exp20\\histo", 59);
//	_histo.load(YARPString(tmp));

	// log file
	char logFilename[256];
	const char *root = GetYarpRoot();
	ACE_OS::sprintf (logFilename, "%s/conf/babybot/handforward.dat\0", root);
	_log.append(logFilename);
	
	YARPLogpolar _mapper;

	int _nHistos = 0;
	int _nFrames = 0;
	while (true)
	{
		// blocking on image port
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
		
		// update handlocalization class
		_handLocalization.update(_arm, _head);

		// poll arm segmentation data
		if(_armSegmentationPort.Read(0))
		{
			_armSegmentationPort.Content().display();
			_handLocalization.update(_arm, _head);
			
			_handLocalization.prepareRemoteTrainData(_armSegmentationPort.Content(), _outputTrainBottle1, _outputTrainBottle2);

			YARPShapeEllipse tmpEl;
			tmpEl = _handLocalization.query(_arm, _head);
			dumpError(tmpEl, _armSegmentationPort.Content());
			
			_outputPortTrain1.Content() = _outputTrainBottle1;
			_outputPortTrain2.Content() = _outputTrainBottle2;
			_outputPortTrain1.Write(1);
			_outputPortTrain2.Write(1);
		}
		
		////////////////////////////////////
		_left.Refer (_inPortImage.Content());
		// reconstruct color
		_mapper.ReconstructColor (_left, _leftColored);
		YARPColorConverter::RGB2HSV(_leftColored, _leftHSV);
		_histo.backProjection(_leftHSV, _outSeg);
		
		YARPShapeEllipse tmpEl1,tmpEl2;
		tmpEl1 = _handLocalization.query(_arm, _head);
		tmpEl2 = _handLocalization.queryPrediction();
		_segmenter.mergeColor(_leftColored, _outSeg, tmpEl1);

		YARPImageOf<YarpPixelBGR> &tmpSeg = _segmenter.getImage();
		YARPSimpleOperation::DrawCross(tmpSeg, tmpEl2.x, tmpEl2.y, YarpPixelBGR(0, 255, 0), 10, 2);
						
		// colored image
		_outPortColor.Content().Refer(tmpSeg);
		_outPortColor.Write();
			
		// histo backprojection
		_outPortBackprojection.Content().Refer(_outSeg);
		_outPortBackprojection.Write();

		// send hand position
		_handPosition(1) = (tmpEl1.x - 128);
		_handPosition(2) = (tmpEl1.y - 128);
		_outputPortHandPosition.Content() = _handPosition;
		_outputPortHandPosition.Write();

		// send hand predicted
		_handPosition(1) = (tmpEl2.x - 128);
		_handPosition(2) = (tmpEl2.y - 128);
		_outputPortHandPredicted.Content() = _handPosition;
		_outputPortHandPredicted.Write();


		// poll remote learning ports
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

	_log.close();
	return 0;
}

void dumpError(YARPShapeEllipse &ellipse, YARPBottle &segData)
{
	int ix;
	int iy;
	double a11, a12, a22;

	segData.readInt(&ix);
	segData.readInt(&iy);
	segData.readFloat(&a11);
	segData.readFloat(&a12);
	segData.readFloat(&a22);

	_log.dump(ix);
	_log.dump(iy);
	_log.dump(a11);
	_log.dump(a12);
	_log.dump(a22);
	_log.dump(ellipse.x);
	_log.dump(ellipse.y);
	_log.dump(ellipse.a11);
	_log.dump(ellipse.a12);
	_log.dump(ellipse.a22);
	_log.newLine();
	_log.flush();
}

////////// print frame status
void printFrameStatus(int n)
{
	if (n%1000 == 0)
		ACE_OS::printf("HeadFrame# %d\tArmFrame# %d\n", __headCounter, __armCounter);
}

