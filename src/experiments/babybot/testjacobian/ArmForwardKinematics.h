#ifndef __ArmForwardKinematicshh__
#define __ArmForwardKinematicshh__

#include <YARPLogFile.h>
#include <YARPConicFitter.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPBPNNet.h>
#include <YARPBabybotHeadKin.h>

#include <YARPPort.h>
#include <YARPBottleContent.h>
#include <./conf/YARPBabybotVocab.h>
#include <YARPRndUtils.h>
#include <vector>

class ArmForwardKinematics
{
public:
	// f1 and f2 are nnet config files; even on a non-trained network
	// they are required to speficy the network structur (#input, #nlayers, #noutput...)
	// if weights and biases are not specified the nnets are randomly initialized
	ArmForwardKinematics(const char *f1, const char *f2);
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

	void loadCenter(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading center nnet from memory:");
		_center.load(p);
	}

	void loadEllipse(YARPBPNNetState &p)
	{
		ACE_OS::printf("Loading ellipse nnet from memory:");
		_ellipse.load(p);
	}

	void randomExploration(int currentX, int currentY);
	void computeJacobian();
	const YMatrix &jacobian(){ return _jacobian; }
	const YMatrix &jacobianInv(){ return _jacobianInv; }
	void update(const YVector &arm, const YVector &head);
	const YVector *getPoints(){return _data;}
	int getNPoints(){return _nPoints;}

	YVector computeCommand(YVector initialArm, int targetX, int targetY);
	
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

	const std::vector<int *> &getTrajectory()
	{ return _trajectory; }
	
private:
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

		double p[3];
		_ellipse.sim(arm.data(), p);

		el.x = predx;		
		el.y = predy;	
		el.a11 = p[0]; 
		el.a12 = p[1]; 
		el.a22 = p[2]; 
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
	vector<int *> _trajectory;
};

#endif