// testjacobian.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPConfig.h>

#include <yarp/YARPParseParameters.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPLogpolar.h>

#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPDebugUtils.h>

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

void plotTrajectory(const Trajectories &tr, YARPImageOf<YarpPixelBGR> &img, YarpPixelBGR color);
void receiveTrajectory(YARPInputPortOf<YARPBabyBottle> &port,  Trajectories &tr);

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

inline bool pollPort(YARPInputPortOf<YARPBabyBottle> &port, int *x, int *y)
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

void dumpError(YARPShapeEllipse &ellipse, YARPBabyBottle &segData);
YARPConicFitter _conicFitter;

const double __armInitial[] = {7, 15, 15, 0, 0, 0};
const double __headInitial[] = {-4, -20, -15, -8, -8};

int main(int argc, char* argv[])
{
	// parse parameters
	double threshold = __threshold;
	bool plotPrediction = true;
	bool plotActual= true;

	if (!YARPParseParameters::parse(argc, argv, "-threshold", &threshold))
		threshold = __threshold;
	
	if (!YARPParseParameters::parse(argc, argv, "-prediction"))
		plotPrediction = false;

	if (!YARPParseParameters::parse(argc, argv, "-position"))
		plotActual = false;

	/// form config name
	const char *root = GetYarpRoot();
	char tmp[256];
	char tmp1[256];
	char tmp2[256];
	ACE_OS::sprintf (tmp, "%s/conf/babybot/handforward.dat", root);

	//// PORT LIST
	DECLARE_INPUT_PORT (YARPGenericImage, _inPortImage, YARP_MCAST);
	DECLARE_OUTPUT_PORT (YARPGenericImage, _outPortColor, YARP_UDP);

	DECLARE_INPUT_PORT (YARPControlBoardNetworkData, _armPort, YARP_UDP);
	DECLARE_INPUT_PORT (YVector, _headPort, YARP_UDP);
	DECLARE_INPUT_PORT (YARPBabyBottle, _reachingTrajectoryPort, YARP_TCP);

	DECLARE_INPUT_PORT (YARPBabyBottle, _inputTarget, YARP_UDP);
	DECLARE_OUTPUT_PORT (YARPBabyBottle, _armCommand, YARP_UDP);
	/////////////////////

	/// REGISTER
	_inPortImage.Register("/testjacobian/i:img", "Net1");
	_headPort.Register("/testjacobian/head/i");
	_armPort.Register("/testjacobian/arm/i");
	_inputTarget.Register("/testjacobian/target/i");
	_outPortColor.Register("/testjacobian/o:img");
	_armCommand.Register("/testjacobian/arm/o");
	_reachingTrajectoryPort.Register("/testjacobian/reaching/i");
	/////////////////////
	
	Trajectories _trajectory;

	YARPLogpolar _mapper;
	YARPBPNNet _openLoopReaching;			// reaching nnetwork
	YARPBabybotHeadKin _headKinematics (YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]));		// head kinematics

	ACE_OS::sprintf (tmp1, "%s/conf/babybot/%s", root, __nnetFile1);
	ACE_OS::sprintf (tmp2, "%s/conf/babybot/%s", root, __nnetFile2);
	ArmForwardKinematics _armJacobian(tmp1,tmp2);
	YARPImageOf<YarpPixelMono> _left;
	YARPImageOf<YarpPixelBGR> _leftColored;
	_left.Resize(_stheta, _srho);
	_leftColored.Resize(_stheta, _srho);

	// READ NNET FROM FILE
	char filename1[255];
	ACE_OS::sprintf (filename1, "%s/conf/babybot/%s", root, "reaching.ini");
	if (_openLoopReaching.load(filename1)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename1);
		exit(-1);
	}
	ACE_OS::printf("Reading neural network from %s\n", filename1);
	
	YVector _arm(6, __armInitial);
	_arm *= degToRad;
	YVector _head(5, __headInitial);
	_head *= degToRad;
	YVector _handPosition(2);
	YARPBabyBottle _bottle;

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

		receiveTrajectory(_reachingTrajectoryPort,  _trajectory);

//		_head = YVector(5, __headInitial);
//		_head = _head*degToRad;
//		_arm = YVector(6, __armInitial);
//		_arm = _arm*degToRad;

		YARPShapeEllipse tmpEl;
		tmpEl = _armJacobian.query(_arm, _head);
	
		// current
		_conicFitter.plotEllipse(tmpEl, _outSeg2, YarpPixelBGR(255, 0, 0));
		YARPSimpleOperation::DrawCross(_outSeg2, tmpEl.x, tmpEl.y, YarpPixelBGR(255, 0, 0), 5, 1);


		// update handlocalization class
		
		_armJacobian.update(_arm, _head);
		_headKinematics.update(_head);
		

		YVector tmpArm = _arm;
		tmpArm(4) = 0.0;
		tmpArm(5) = 0.0;
		tmpArm(6) = 0.0;

		YVector newCmd(6);

		if(active)
		{
			// start a loop 
			active = false;
			
			// draw desired position
			YARPSimpleOperation::DrawCross(_outSeg2, x, y, YarpPixelBGR(0, 255, 0), 5, 1);

			printf("Computing Jacobian...\n");
			// x = 128;
			// y = 128;
			_armJacobian.computeJacobian(x, y);
			int h = 0;
			const YVector *tmpPoints = _armJacobian.getPoints();
			for(h = 0; h < _armJacobian.getNPoints(); h++)
			{
				YARPSimpleOperation::DrawCross(_outSeg2, tmpPoints[h](1), tmpPoints[h](2), YarpPixelBGR(128, 0, 0), 3, 1);
			}

			/*
			printf("done\n");
		
			printf("jacobianInv:\n %lf\t%lf\n", _handLocalization.jacobianInv()(1,1), _handLocalization.jacobianInv()(1,2));
			printf("%lf\t%lf\n", _handLocalization.jacobianInv()(2,1), _handLocalization.jacobianInv()(2,2));
			*/
			////////////////////////////////////
			// _left.Refer (_inPortImage.Content());
			// reconstruct color
			// _mapper.ReconstructColor (_left, _leftColored);

			const Y3DVector &cart = _headKinematics.fixationPolar();
			_openLoopReaching.sim(cart.data(), newCmd.data());
			_armJacobian.computeJacobian(x,y);		// compute from center
			YVector tmpArm(6);
			tmpArm(1) = newCmd(1)+5*degToRad;	 	//copy 1 joint from map
			tmpArm(2) = _arm(2);
			tmpArm(3) = _arm(3);
			tmpArm(4) = _arm(4);
			tmpArm(5) = _arm(5);
			tmpArm(6) = _arm(6);

			// overwrite command
			newCmd = _armJacobian.computeCommandThreshold(tmpArm , x, y);	// to center
		
			/*
			// trajectory
			tmpArm(1) = -1*degToRad;  // pick a fixed shoulder joint position
			newCmd = _handLocalization.computeCommand(tmpArm, x, y);
			newCmd(1) = tmpArm(1);	// override shoulder
			newCmd(4) = 0.0;
			newCmd(5) = 0.0;
			newCmd(5) = 0.0;*/
		
			// done, send arm command
			_bottle.reset();
			_bottle.setID(YBVMotorLabel);
			_bottle.writeVocab(YBVArmForceNewCmd);
			_bottle.writeYVector(newCmd);

			_bottle.display();

			_armCommand.Content() = _bottle;
			_armCommand.Write(1);
		}
		
		plotTrajectory(_armJacobian.getTrajectory(), _outSeg2, YarpPixelBGR(0,49,130));
		plotTrajectory(_trajectory, _outSeg2, YarpPixelBGR(130,0,25));
		_outPortColor.Content().Refer(_outSeg2);
		_outPortColor.Write();
			
		printFrameStatus(_nFrames);
		_nFrames++;
	}

	return 0;
}

void dumpError(YARPShapeEllipse &ellipse, YARPBabyBottle &segData)
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

void plotTrajectory(const Trajectories &v, YARPImageOf<YarpPixelBGR> &img, YarpPixelBGR color)
{
	YARPShapeEllipse tmpEl;

	for (int i = 0; i < v.length; i++)
	{
		tmpEl.x = v.pixels[i][0];
		tmpEl.y = v.pixels[i][1];
		YARPSimpleOperation::DrawCross(img, tmpEl.x, tmpEl.y, color, 5, 1);

//		YARPDebugUtils::print(v.arm[i]*radToDeg);

	}

	// the last one
	color.r+=color.r*0.5;
	if (color.r >255)
		color.r = 255;
	color.g+=color.g*0.5;
	if (color.g >255)
		color.g = 255;
	color.b+=color.b*0.5;
	if (color.b >255)
		color.b = 255;
	YARPSimpleOperation::DrawCross(img, tmpEl.x, tmpEl.y, color, 5, 1);
}


void receiveTrajectory(YARPInputPortOf<YARPBabyBottle> &port,  Trajectories &tr)
{
	if (!port.Read(0))
		return;

	int size;
	int length;

	YARPBabyBottle &content = port.Content();
	content.readInt(&size);
	content.readInt(&length);

	tr.resize(size);
	tr.length = length;

	printf("Received a trajectory of %d elem\n", length);

	bool ret;
	int x;
	int y;
	YVector arm;
	int i;
	for(i = 0; i < (length-1); i++)
	{
		ret = content.readInt(&x);
		ret = ret && content.readInt(&y);
		ret = ret && content.readYVector(arm);

		if (ret)
		{
			tr.pixels[i][0] = x;
			tr.pixels[i][1] = y;
			tr.arm[i] = arm;
		}
		else
			printf("Error reading bottle; check sender\n");
	}

	ret = content.readInt(&x);
	ret = ret && content.readInt(&y);
	ret = ret && content.readYVector(arm);

	if (ret)
	{
		tr.pixels[i][0] = x;
		tr.pixels[i][1] = y;
		tr.arm[i] = arm;
	}
	else
		printf("Error reading bottle; check sender\n");

	// dump arm command
	for(i = 0; i < tr.length; )
	{
		// dump trajectory
		YARPDebugUtils::print(tr.arm[i]*radToDeg);
		i+=2;
	}
}
