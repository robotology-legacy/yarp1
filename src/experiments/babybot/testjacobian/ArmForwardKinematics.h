#ifndef __ArmForwardKinematicshh__
#define __ArmForwardKinematicshh__

#include <yarp/YARPLogFile.h>
#include <yarp/YARPConicFitter.h>
#include <yarp/YARPBabyBottle.h>
#include <yarp/YARPControlBoardNetworkData.h>
#include <yarp/YARPBPNNet.h>
#include <yarp/YARPBabybotHeadKin.h>

#include <yarp/YARPPort.h>
#include <yarp/YARPConfigRobot.h>
#include <yarp/YARPRndUtils.h>

#include <vector>

const int __nj = 6;

class Trajectories
{
public:
	Trajectories()
	{
		size = 0; 
		length = 0;
	}
	~Trajectories()
	{
		int i;
		if (size>0)
		{
			for (i = 0; i < size; i++)
				delete [] pixels[i];
		}
	}
	void resize(int s)
	{
		size = s;
		length = s;
		for(int i = 0; i < s; i++)
		{
			int *newPixel = new int[2];
			newPixel[0] = 0;
			newPixel[1] = 1;
			pixels.push_back(newPixel);
			
			YVector newArm;
			newArm.Resize(__nj);
			newArm = 0.0;
			arm.push_back(newArm);
		}
	}

	int size;
	int length;
	std::vector<int *> pixels;
	std::vector<YVector> arm;
};


class ArmForwardKinematics
{
public:
	ArmForwardKinematics();
	ArmForwardKinematics(const char *f1);
	~ArmForwardKinematics();

	void query(YVector &arm, YVector &head, YARPShapeEllipse &el)
	{
		_query(arm, head, el);
	}

	YARPShapeEllipse query(YVector &arm, YVector &head)
	{
		YARPShapeEllipse el;
		query(arm, head, el);
		return el;
	}

	void computeJacobian(int x0, int y0);
	const YMatrix &jacobian(){ return _jacobian; }
	const YMatrix &jacobianInv(){ return _jacobianInv; }
	void update(const YVector &arm, const YVector &head);
	const YVector *getPoints(){return _data;}
	int getNPoints(){return _nPoints;}

	int load(const char *f1);
	
	YVector computeCommandThreshold(YVector initialArm, int targetX, int targetY);
	YVector computeCommandFixedSteps(YVector initialArm, int targetX, int targetY);
	
	YVector plan(const YVector &dT)
	{
		YVector tmp(2);
		YVector tmpArm(6);
		tmp = _jacobianInv * dT;
		
		tmpArm(6) = 0.0;
		tmpArm(2) = tmp(1);
		tmpArm(3) = tmp(2);
		
		return tmpArm;
	}

	const Trajectories &getTrajectory()
	{ return _trajectory; }
	
private:
	void _init();
	void _randomExploration(int x0, int y0);
	void _query(const YVector &arm, const YVector &head, int &x, int &y)
	{
		// compute _v()
		_center.sim(arm.data(),	// uses only the first 3 joints
				    _v.data());

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, x, y);
	}
	void _query(const YVector &arm, const YVector &head, YARPShapeEllipse &el)
	{
		// compute _v()
		_center.sim(arm.data(),	// uses only the first 3 joints
				    _v.data());

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);
		//////////////////////////////////////

		el.x = predx;		
		el.y = predy;	
		el.a11 = 0; 
		el.a12 = 0; 
		el.a22 = 0; 
	}

	void _dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse);

	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	
	YARPBPNNet _center;
	YARPBPNNet _ellipse;

	YARPBabybotHeadKin _gaze;
	YVector _v;
	YVector _head;
	YVector _arm;
	int _nPoints;
	YVector *_data;
	YVector *_deltaQ;
	/// LS
	YVector _b1;	// b least squares
	YVector _b2;	// b least squares
	YVector _x1;	// x least squares
	YVector _x2;	// x least squares
	YMatrix _A;		// A matrix least squares
	YMatrix _jacobian;
	YMatrix _jacobianInv;

	YARPRndGaussVector _random;
	Trajectories _trajectory;
};

#endif