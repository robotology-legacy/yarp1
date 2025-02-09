#include <yarp/YARPDebugUtils.h>
#include "ArmForwardKinematics.h"

const int __nPoints = 10;
const double __explStd = 5;
const int __nSteps = 1500;
const int __pixelThreshold = 4;

ArmForwardKinematics::ArmForwardKinematics():
_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) )
{
	_init();
}

ArmForwardKinematics::ArmForwardKinematics(const char *f1, const char *f2):
_gaze ( YMatrix (_dh_nrf, 5, DH_left[0]), YMatrix (_dh_nrf, 5, DH_right[0]), YMatrix (4, 4, TBaseline[0]) )
{
	// load nnets if name was specified
	if (load(f1) != YARP_OK)
	{
		ACE_OS::printf("Error reading nnet file in %s", f1);
		exit(-1);
	}

	if (loadEllipse(f2) != YARP_OK)
	{
		ACE_OS::printf("Error reading nnet file in %s", f2);
		exit(-1);
	}
	_init();
}

int ArmForwardKinematics::load(const char *f1)
{
	return _center.load(f1);
}

int ArmForwardKinematics::loadEllipse(const char *f1)
{
	return _ellipse.load(f1);
}

void ArmForwardKinematics::_init()
{
	_nPoints = __nPoints;
	_v1.Resize(3);
	_v2.Resize(3);

	_random.resize(1, 0.0, __explStd*degToRad);

	_data = new YVector[_nPoints];
	for(int k = 0; k < _nPoints; k++)
	{
		_data[k].Resize(2);
		_data[k] = 0.0;
	}

	_A.Resize(_nPoints, 2);
	_b1.Resize(_nPoints);
	_b2.Resize(_nPoints);
	_x1.Resize(2);
	_x2.Resize(2);
	_jacobian.Resize(2,2);
	_jacobianInv.Resize(2,2);

	_trajectory.resize(__nSteps);
}

ArmForwardKinematics::~ArmForwardKinematics()
{
	_log.close();
}

void ArmForwardKinematics::update(const YVector &arm, const YVector &head)
{
	_head = head;	// store current head and arm position
	_arm = arm;		// 

	_gaze.update(_head);
}

void ArmForwardKinematics::_randomExploration(int x0, int y0)
{
	int i = 0;
	YVector deltaQ;
	YVector tmp;
	int tmpX;
	int tmpY;
	tmp = _arm;
	for(i = 0; i<_nPoints; i++)
	{
		// perform a step
		double tmpDelta1 = _random.getScalar();
		double tmpDelta2 = _random.getScalar();

		tmp(2) = _arm(2) + tmpDelta1;
		tmp(3) = _arm(3) + tmpDelta2;
	//	tmp(3) = _arm(3) + tmpDelta1;
	
	//	_query(tmp, _head, tmpX, tmpY); // center
		_queryFingers(tmp, _head, tmpX, tmpY);
		
		_data[i](1) = tmpX;
		_data[i](2) = tmpY;
		_b1(i+1) = tmpDelta1; // tmpX;
		_b2(i+1) = tmpDelta2; // tmpY;
		_A(i+1,1) = tmpX-x0; // tmpDelta1;
		_A(i+1,2) = tmpY-y0; // tmpDelta2;
	}
}

void ArmForwardKinematics::computeJacobian(int x0, int y0)
{
	_randomExploration(x0, y0);

	VisDMatrixSVD(_A, _b1, _x1);
	VisDMatrixSVD(_A, _b2, _x2);

	_jacobianInv(1,1) = _x1(1);
	_jacobianInv(1,2) = _x1(2);

	_jacobianInv(2,1) = _x2(1);
	_jacobianInv(2,2) = _x2(2);

}

YVector ArmForwardKinematics::computeCommandFixedSteps(YVector initialArm, int targetX, int targetY)
{
	YVector dT(2);
	YVector &tmpArm = initialArm;
	// YARPShapeEllipse tmpEl = query(tmpArm, _head);	//center
	int tmpX, tmpY;
	queryFingers(tmpArm, _head, tmpX, tmpY);
	int i;
	for(i = 0; i < __nSteps; i++)
	{
		// planning trajectory
		// dT(1) = targetX-tmpEl.x;	// center
		// dT(2) = targetY-tmpEl.y;	//center
		dT(1) = targetX-tmpX;
		dT(2) = targetY-tmpY;

		YVector dQ = plan(dT);
		dQ(2) = dQ(2) * 0.05;
		dQ(3) = dQ(3) * 0.05;
		tmpArm = tmpArm+dQ;

		update(tmpArm, _head);
		// tmpEl = query(tmpArm, _head);	//center
		queryFingers(tmpArm, _head, tmpX, tmpY);
	
		// current
		// _trajectory.pixels[i][0] = tmpEl.x;	//center
		// _trajectory.pixels[i][1] = tmpEl.y;	//center
		_trajectory.pixels[i][0] = tmpX;
		_trajectory.pixels[i][1] = tmpY;

		_trajectory.arm[i] = tmpArm;

		// this is to recompute the jacobian every time
		//computeJacobian(tmpEl.x, tmpEl.y);	//center
		computeJacobian(tmpX, tmpY);	//center
	}

	_trajectory.length = i;

	return tmpArm;
}

YVector ArmForwardKinematics::computeCommandThreshold(YVector initialArm, int targetX, int targetY)
{
	ACE_OS::printf("Computing trajectory");
	YVector dT(2);
	YVector &tmpArm = initialArm;
	// YARPShapeEllipse tmpEl = query(tmpArm, _head);	//center
	int tmpX, tmpY;
	queryFingers(tmpArm, _head, tmpX, tmpY);
	int i = 0;
	bool done = false;
	while(!done)
	{	
		// planning trajectory
		// dT(1) = targetX-tmpEl.x;	//center
		// dT(2) = targetY-tmpEl.y;	//center
		dT(1) = targetX-tmpX;
		dT(2) = targetY-tmpY;
		YVector dQ = plan(dT);
		dQ(2) = dQ(2) * 0.05;
		dQ(3) = dQ(3) * 0.05;
		tmpArm = tmpArm+dQ;

		update(tmpArm, _head);
		// tmpEl = query(tmpArm, _head);	//center
		queryFingers(tmpArm, _head, tmpX, tmpY);
	
		// current
		// _trajectory.pixels[i][0] = tmpEl.x;	//center
		// _trajectory.pixels[i][1] = tmpEl.y;	//center
		_trajectory.pixels[i][0] = tmpX;
		_trajectory.pixels[i][1] = tmpY;
		_trajectory.arm[i] = tmpArm;

		// this is to recompute the jacobian every time
		// computeJacobian(tmpEl.x, tmpEl.y);		//center
		computeJacobian(tmpX, tmpY);		//center

		if ( (dT(1)*dT(1) + dT(2)*dT(2))<__pixelThreshold)
			done = true;
		
		// it
		if (i==(__nSteps-1))
		{
			ACE_OS::printf("given up ! (max # steps reached)\n");
			done = true;
			_trajectory.length = __nSteps;
			return initialArm;
		}
		i++;
		ACE_OS::printf(".");
	}
	
	_trajectory.length = i-1;
	ACE_OS::printf("done !\nTrajectory computed in %d steps\n", _trajectory.length);

	YARPDebugUtils::print(tmpArm, "Arm command:");
	return tmpArm;
}