#ifndef __ARMMAP__
#define __ARMMAP__

#include <yarp/YARPBabybotHead.h>
#include <yarp/YARPRndUtils.h>
#include <time.h>
#include <yarp/YARPBPNNet.h>
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
const double __preparePosition[] = { 10*degToRad, 0.0, 0.0, 0*degToRad, 0.0, -70.0*degToRad};

// offset in the shoulder to reach "from above"
const double __shoulderOffset1 = 25*degToRad;
const double __shoulderOffset2 = 5*degToRad;
// to turn the hand downwardly
const double __wrist1 = -10*degToRad;	
const double __wrist2 = -30*degToRad;
const double __wrist3 = -170*degToRad;

// offset in the retinal space, for the reaching
const double __azimuthOffset = -6*degToRad;
const double __elevationOffset = -7*degToRad;
const double __distanceOffset = -40;

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

const int __trajectoryLength = 3;

class ArmMap: public YARPInputPortOf<YARPBabyBottle>
{
public:
	ArmMap(const char *nnetFile);
	~ArmMap();

	void load(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading reaching map from memory:");
		_nnet.load(p);
	}

	void query(const YVector &arm, const YVector &head);
	const YVector &reachingCmd()
	{ return _command; }
	const YVector &prepareCmd()
	{ return _prepare; }
	void learn(const YVector &head, const YVector &arm);

	const YVector &getCommand(int n);
	
private:
	bool _checkLearnCondition();
	bool _checkReachingCondition();
	void _formTrajectory(const YVector &cmd);

	YARPBPNNet _nnet;
	ArmForwardKinematics _fkinematics;
		
	YARPOutputPortOf<YARPBabyBottle> _outPortRemoteLearn;

	virtual void OnRead(void);

	ATR _atr;
	NoiseModule _noise;

	YARPBabybotHeadKin _headKinematics;
	YARPBabyBottle _bottle;
	YVector _command;
	YVector _prepare;
	YVector *_trajectory;

	int _nUpdated;
	int _trajectoryLength;
	enum mode {atnr = 0, learning = 1, reaching = 2};
	int _mode;
};

#endif