#ifndef __HANDKINEMATICS__
#define __HANDKINEMATICS__

#include <YARPLogFile.h>
#include <YARPConicFitter.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPBPNNet.h>
#include <YARPBabybotHeadKin.h>

class HandKinematics
{
public:
	HandKinematics();
	~HandKinematics();

	void learn(YVector &arm, YVector &head, YARPBottle &newPoint);
	
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

private:
	void _query(const YVector &arm, const YVector &head, YARPShapeEllipse &el)
	{
		// update head kinematics
		_gaze.update(head);

		// compute _v()
		_center.sim(arm.data(),	// uses only the first 3 joints
				    _v.data());

		// compute retinal position
		int predx = 0, predy = 0;
		_gaze.intersectRay (YARPBabybotHeadKin::KIN_LEFT_PERI, _v, predx, predy);

		//////////////////////////////////////

		// parameters.sim(tmp, p);

		el.rho = predx;		//c[0];
		el.theta = predy;	//c[1];
		el.a11 = /*p[0];*/ 0.005;
		el.a12 = /*p[1];*/ 0;
		el.a22 = /*p[2];*/ 0.005;
	}

	void _dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse);
	
	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	
	YARPBPNNet _center;
	YARPBPNNet _parameters;

	YARPBabybotHeadKin _gaze;
	YVector _v;
	int _npoints;
};

#endif