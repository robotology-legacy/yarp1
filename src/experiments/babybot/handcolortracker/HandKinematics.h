#ifndef __HANDKINEMATICS__
#define __HANDKINEMATICS__

#include <YARPLogFile.h>
#include <YARPConicFitter.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPControlBoardNetworkData.h>
#include <YARPBPNNet.h>

class HandKinematics
{
public:
	HandKinematics();
	~HandKinematics();

	void learn(YVector &arm, YVector &head, YARPBottle &newPoint);
	
	/*
	void query(YARPImageOf<YarpPixelMono> &in)
	{
		YARPShapeEllipse el;

		_query(_armPosition, el);
		_fitter.plotEllipse(el, in);
	}*/

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
	void _query(const YVector &arm, const YVector &hand, YARPShapeEllipse &el)
	{
		double tmp[3];
		double c[2];
		double p[3];
		tmp[0] = arm(1);
		tmp[1] = arm(2);
		tmp[2] = arm(3);
		center.sim(tmp, c);
		parameters.sim(tmp, p);
	
		el.rho = c[0];
		el.theta = c[1];
		el.a11 = /*p[0];*/ 0.005;
		el.a12 = /*p[1];*/ 0;
		el.a22 = /*p[2];*/ 0.005;
	}

	void _dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse);
	
	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	
	YARPBPNNet center;
	YARPBPNNet parameters;

	int _npoints;
};

#endif