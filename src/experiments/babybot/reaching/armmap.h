#ifndef __ARMMAP__
#define __ARMMAP__

//#define TEST_REACHING

#include <yarp/YARPBabybotHead.h>
#include <yarp/YARPRndUtils.h>
#include <time.h>
#include <yarp/YARPBPNNet.h>
#include <yarp/RFNet.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPMatrix.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPBabybotHeadKin.h>

#include "../testjacobian/ArmForwardKinematics.h"

const int __nJointsArm = 6;
const double __front = +15*degToRad;
const double __left = -15*degToRad;
const double __right = +15*degToRad;

const int __nATN = 3;
const int __iF = 1;
const int __iL = 0;
const int __iR = 2;

const double __arm[__nATN][__nJointsArm] = {
							{0.0, -10.0*degToRad, 0.0, 0.0, 0.0, 0.0},
							{0.0, 15.0*degToRad, 0.0, 0.0, 0.0, 0.0},
							{0.0, 35.0*degToRad, 15.0*degToRad, 0.0, 0.0, 0.0},
};

const double __maxRnd[] = {5.0*degToRad, 5.0*degToRad, 5.0*degToRad, 0.0, 0.0, 0.0};
const double __minRnd[] = {-5.0*degToRad, -5.0*degToRad, -5.0*degToRad, 0.0, 0.0, 0.0};

// command used to prepare reaching
// const double __preparePosition[] = { 15*degToRad, 0.0, 0.0, 0*degToRad, 0.0, -70.0*degToRad};
// const double __preparePosition[] = { 15*degToRad, 0.0, 0.0, 0*degToRad, 0.0, 0.0*degToRad};
const double __preparePosition[] = { 5*degToRad, 0.0, 0.0, 0*degToRad, 0.0, 0.0*degToRad};


#ifndef TEST_REACHING
	// offset in the shoulder to reach "from above"
	const double __shoulderOffset1 = 40*degToRad;
	const double __shoulderOffset2 = 10*degToRad;
	// offset in the shoulder to fix offset in the reaching
	// (tappullo)
	const double __armOffset1 = 0*degToRad;
	const double __armOffset2 = 0*degToRad;
	const double __foreArmOffset1 = 0;//-8*degToRad;
	const double __foreArmOffset2 = 0;//-8*degToRad;

	const double __wrist1a = 0*degToRad;	
	const double __wrist2a = 0*degToRad;
	const double __wrist3a = -170*degToRad;
	// const double __wrist3 = 0*degToRad;

	const double __wrist1b = 0*degToRad;	
	const double __wrist2b = -45*degToRad;
	const double __wrist3b = -170*degToRad;
#else 
	// offset in the shoulder to reach "from above"
	const double __shoulderOffset1 = 7*degToRad;
	const double __shoulderOffset2 = 7*degToRad;
	// offset in the shoulder to fix offset in the reaching
	// (tappullo)
	const double __armOffset1 = 0*degToRad;
	const double __armOffset2 = 0*degToRad;
	const double __foreArmOffset1 = 0*degToRad;
	const double __foreArmOffset2 = 0*degToRad;

	const double __wrist1a = 0*degToRad;	
	const double __wrist2a = 0*degToRad;
	const double __wrist3a = 0*degToRad;
	// const double __wrist3 = 0*degToRad;

	const double __wrist1b = 0*degToRad;	
	const double __wrist2b = 0*degToRad;
	const double __wrist3b = 0*degToRad;
#endif

// to turn the hand downwardly
// const double __wrist1 = -10*degToRad;	
// const double __wrist2 = -30*degToRad;
// const double __wrist3 = -170*degToRad;



// offset in the head space, for reaching
const double __azimuthOffset = -6*degToRad;
const double __elevationOffset = -20*degToRad;
const double __distanceOffset = -60;

// very simple class to implement ATR
class ATR
{
public:
	ATR()
	{
		_arm = new YVector[__nATN];

		for(int i = 0; i < __nATN; i++)
		{
			_arm[i].Resize(__nJointsArm, __arm[i]);
		}
	}
	
	~ATR()
	{
		delete [] _arm;
	}

	const YVector &query(const YVector &head)
	{
		if (fabs(head(_joints::neckPan+1)) <= __front)
			return _arm[__iF];
		else if (head(_joints::neckPan+1) > __right)
			return _arm[__iR];
		else if (head(_joints::neckPan+1) < __left)
			return _arm[__iL];

		return _arm[0];
	}

private:
	YVector *_arm;
};

class NoiseModule: public YARPRndVector
{
public:
	NoiseModule()
	{}
	
	NoiseModule(const YVector &max, const YVector &min): YARPRndVector(max, min)
	{}
	
	const YVector &query()
	{
		return getVector();
	}
};

const int __trajectoryLength = 4;

class ArmMap: public YARPInputPortOf<YARPBabyBottle>
{
public:
	ArmMap(const char *reachingNNet, const char *handlocNNet, const char *ellipseNNet);
	~ArmMap();

	void load(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading reaching map from memory:");
//		_nnet.load(p);
	}

	bool query(const YVector &arm, const YVector &head);
	const YVector &reachingCmd()
	{ return _command; }
	const YVector &prepareCmd()
	{ return _prepare; }
	void learn(const YVector &head, const YVector &arm);

	const YVector &getCommand(int n);

private:
	bool _checkLearnCondition();
	bool _checkReachingCondition();
	void _formTrajectory(const YVector &cmd, const YVector &cmdCL);
	void _sendTrajectory();
	bool _checkReachability(const Y3DVector &cart);


//	YARPBPNNet _nnet;
	RFNet	   _rfnet;

	ArmForwardKinematics _fkinematics;
		
	YARPOutputPortOf<YARPBabyBottle> _outPortRemoteLearn;
	YARPOutputPortOf<YARPBabyBottle> _outPortArmTrajectory;

	virtual void OnRead(void);

	ATR _atr;
	NoiseModule _noise;

	YARPBabybotHeadKin _headKinematics;
	YARPBabyBottle _bottle;
	YVector _command;
	YVector _commandCL;
	YVector _prepare;
	YVector *_trajectory;

	int _nUpdated;
	int _trajectoryLength;
	enum mode {atnr = 0, learning = 1, reaching = 2};
	int _mode;
};

#endif