// testjacobian.cpp : Defines the entry point for the console application.
//

#include <YARPParseParameters.h>
#include <YARPIntegralImage.h>
#include <YARPDIBConverter.h>
#include <YARPSimpleOperations.h>
#include <YARPColorConverter.h>
#include <YARPBlobDetector.h>
#include <YARPImageFile.h>
#include <YARPLogpolar.h>

#include <YARPControlBoardNetworkData.h>
#include <YARPVectorPortContent.h>
#include <YARPBottleContent.h>
#include <YARPPort.h>

#include <YARPBlobDetector.h>

#include <YARPHistoSegmentation.h>
#include <vector>

#include "ArmForwardKinematics.h"

using namespace _logpolarParams;

YARPLogpolar _mapper;

const char *__nnetFile1 = "handfk1.ini";
const char *__nnetFile2 = "handfk2.ini";

const int __xOffset = 0;
const int __yOffset = -8;

const double __threshold = 150000;
int __armCounter = 0;
int __headCounter = 0;

#define DECLARE_INPUT_PORT(TYPE, NAME, PROTOCOL) YARPInputPortOf<TYPE> NAME(YARPInputPort::DEFAULT_BUFFERS, PROTOCOL)
#define DECLARE_OUTPUT_PORT(TYPE, NAME, PROTOCOL) YARPOutputPortOf<TYPE> NAME(YARPOutputPort::DEFAULT_OUTPUTS, PROTOCOL)

using std::vector;

void plotTrajectory(const vector<int *> &tr, YARPImageOf<YarpPixelBGR> &img);


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

inline bool pollPort(YARPInputPortOf<YARPBottle> &port, int *x, int *y)
{
	bool ret = false;
	if (port.Read(0))
	{
		port.Content().readInt(x);
		port.Content().readInt(y);

		printf("Received new target (%d, %d)\n", *x, *y);
		ret = true;
	}

	return ret;
}

void printFrameStatus(int n);

void dumpError(YARPShapeEllipse &ellipse, YARPBottle &segData);
YARPConicFitter _conicFitter;

const double __armInitial[] = {7, 15, 15, 0, 0, 0};
const double __headInitial[] = {-4, -20, -15, -8, -8};

int main(int argc, char* argv[])
{
	// parse parameters
	double threshold = __threshold;
	bool plotPrediction = true;
	bool plotActual= true;

	if (!YARPParseParameters::parse(argc, argv, "threshold", &threshold))
		threshold = __threshold;
	
	if (!YARPParseParameters::parse(argc, argv, "prediction"))
		plotPrediction = false;

	if (!YARPParseParameters::parse(argc, argv, "position"))
		plotActual = false;

	/// form config name
	const char *root = GetYarpRoot();
	char tmp[256];
	ACE_OS::sprintf (tmp, "%s/conf/babybot/handforward.dat", root);

	//// PORT LIST
	DECLARE_INPUT_PORT (YARPGenericImage, _inPortImage, YARP_MCAST);
	DECLARE_OUTPUT_PORT (YARPGenericImage, _outPortColor, YARP_UDP);

	DECLARE_INPUT_PORT (YARPControlBoardNetworkData, _armPort, YARP_UDP);
	DECLARE_INPUT_PORT (YVector, _headPort, YARP_UDP);
	DECLARE_INPUT_PORT (YARPBottle, _inputTarget, YARP_UDP);
	DECLARE_OUTPUT_PORT (YARPBottle, _armCommand, YARP_UDP);
	/////////////////////

	/// REGISTER
	_inPortImage.Register("/testjacobian/i:img", "Net1");
	_headPort.Register("/testjacobian/head/i");
	_armPort.Register("/testjacobian/arm/i");
	_inputTarget.Register("/testjacobian/target/i");
	_outPortColor.Register("/testjacobian/o:img");
	_armCommand.Register("/testjacobian/arm/o");
	/////////////////////
	
	YARPLogpolar _mapper;
	ArmForwardKinematics _handLocalization(__nnetFile1, __nnetFile2);
	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelBGR> _leftColored;
	_left.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);
	
	YVector _arm(6);
	YVector _head(5);
	YVector _handPosition(2);
	YARPBottle _bottle;

	YARPImageOf<YarpPixelBGR> _outSeg2;
	_outSeg2.Resize(256, 256);
	
	int _nHistos = 0;
	int _nFrames = 0;
	
	_outSeg2.Zero();
	_outPortColor.Content().Refer(_outSeg2);
	_outPortColor.Write();

	bool active = false;

	while (true)
	{
		// blocking on image port
		_inPortImage.Read();
		///// poll head and arm positions
		pollPort(_armPort, _arm, &__armCounter);
		pollPort(_headPort, _head, &__headCounter);		

		_inPortImage.Content().Refer(_left);
		_mapper.ReconstructColor(_left, _leftColored);
		_mapper.Logpolar2Cartesian(_leftColored, _outSeg2);
			
		int x,y;
		if (pollPort(_inputTarget, &x, &y)) 
			active = true;

//		_head = YVector(5, __headInitial);
//		_head = _head*degToRad;
//		_arm = YVector(6, __armInitial);
//		_arm = _arm*degToRad;

		YARPShapeEllipse tmpEl;
		tmpEl = _handLocalization.query(_arm, _head);
	
		// current
		_conicFitter.plotEllipse(tmpEl, _outSeg2, YarpPixelBGR(255, 0, 0));
		YARPSimpleOperation::DrawCross(_outSeg2, tmpEl.x, tmpEl.y, YarpPixelBGR(255, 0, 0), 5, 1);


		// update handlocalization class
		_handLocalization.update(_arm, _head);

		YVector tmpArm = _arm;
		YVector newCmd(6);

		tmpArm(1) = -2*degToRad;
		tmpArm(4) = 0.0;
		tmpArm(5) = 0.0;
		tmpArm(6) = 0.0;

		if(active)
		{
			// start a loop 
			active = false;
			
			// draw desired position
			YARPSimpleOperation::DrawCross(_outSeg2, x, y, YarpPixelBGR(0, 255, 0), 5, 1);

			// 
			_handLocalization.randomExploration(tmpEl.x, tmpEl.y);
			int h = 0;
			const YVector *tmpPoints = _handLocalization.getPoints();
			for(h = 0; h < _handLocalization.getNPoints(); h++)
			{
				YARPSimpleOperation::DrawCross(_outSeg2, tmpPoints[h](1), tmpPoints[h](2), YarpPixelBGR(128, 0, 0), 3, 1);
			}

			printf("Computing Jacobian...");
			_handLocalization.computeJacobian();

			/*
			printf("done\n");
		
			printf("jacobianInv:\n %lf\t%lf\n", _handLocalization.jacobianInv()(1,1), _handLocalization.jacobianInv()(1,2));
			printf("%lf\t%lf\n", _handLocalization.jacobianInv()(2,1), _handLocalization.jacobianInv()(2,2));
			*/
			////////////////////////////////////
			// _left.Refer (_inPortImage.Content());
			// reconstruct color
			// _mapper.ReconstructColor (_left, _leftColored);
		
			// trajectory
			newCmd = _handLocalization.computeCommand(tmpArm, x, y);
		
			// done, send arm command
			_bottle.reset();
			_bottle.setID(YBVMotorLabel);
			_bottle.writeVocab(YBVArmNewCmd);
			_bottle.writeYVector(newCmd);

			_armCommand.Content() = _bottle;
			_armCommand.Write(1);
		}
		
		plotTrajectory(_handLocalization.getTrajectory(), _outSeg2);
		_outPortColor.Content().Refer(_outSeg2);
		_outPortColor.Write();
			
		printFrameStatus(_nFrames);
		_nFrames++;
	}

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

/*	_log.dump(ix);
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
	*/
}

////////// print frame status
void printFrameStatus(int n)
{
	if (n%1000 == 0)
		ACE_OS::printf("HeadFrame# %d\tArmFrame# %d\n", __headCounter, __armCounter);
}

void plotTrajectory(const vector<int *> &v, YARPImageOf<YarpPixelBGR> &img)
{
	YARPShapeEllipse tmpEl;

	for (int i = 0; i < v.size(); i++)
	{
		tmpEl.x = v[i][0];
		tmpEl.y = v[i][1];
		YARPSimpleOperation::DrawCross(img, tmpEl.x, tmpEl.y, YarpPixelBGR(255, 128, 0), 5, 1);
	}

	// the last one
	YARPSimpleOperation::DrawCross(img, tmpEl.x, tmpEl.y, YarpPixelBGR(255, 255, 0), 5, 1);
}