#ifndef __ARMMAP__
#define __ARMMAP__

#include <YARPBabybotHead.h>
#include <YARPRnd.h>
#include <time.h>
#include <YARPBPNNet.h>
#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPMatrix.h>
#include <YARPVectorPortContent.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPBabybotHeadKin.h>

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

class NoiseModule: public YARPRnd
{
public:
	NoiseModule(int size = 1, double max = 1.0, double min = 0.0)
	{
		resize(YVector(1, &max), YVector(1, &min));
	}

	NoiseModule(const YVector &max, const YVector &min)
	{
		resize(max, min);
	}

	void resize(int s, const double *max, const double *min)
	{
		resize(YVector(s, max), YVector(s, min));
	}

	void resize(const YVector &max, const YVector &min)
	{
		ACE_ASSERT(max.Length() == min.Length());
		ACE_ASSERT(max.Length() > 0);

		YARPRnd::init((unsigned) time(NULL));

		_size = max.Length();
		_max = max;
		_min = min;
		_data.Resize(_size);
		_random.Resize(_size);
	}

	const YVector &query()
	{
		return getVector();
	}

	const YVector &getVector()
	{
		for(int i = 1; i <= _size; i++)
			_random(i) = (_max(i) - _min(i)) * getNumber() + _min(i);
		
		return _random;
	}

	double getScalar()
	{
		return (_max(1) - _min(1)) * getNumber() + _min(1);
	}

private:
	YVector _max;
	YVector _min;
	YVector _data;
	YVector _random;
	int _size;
};

class ArmMap: public YARPInputPortOf<YARPBottle>
{
public:
	ArmMap(const char *nnetFile);
	~ArmMap();

	void load(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading reaching map from memory:");
		_nnet.load(p);
	}

	const YVector &query(const YVector &head);
	void learn(const YVector &head, const YVector &arm);
	
private:
	bool _checkLearnCondition();
	bool _checkReachingCondition();

	YARPBPNNet _nnet;
		
	YARPOutputPortOf<YARPBottle> _outPortRemoteLearn;

	virtual void OnRead(void);

	ATR _atr;
	NoiseModule _noise;

	YARPBabybotHeadKin _headKinematics;
	YARPBottle _bottle;
	YVector _command;

	int _nUpdated;
};

#endif