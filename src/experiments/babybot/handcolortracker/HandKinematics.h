#ifndef __HANDKINEMATICS__
#define __HANDKINEMATICS__

#include <YARPLogFile.h>
#include <YARPConicFitter.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPControlBoardNetworkData.h>
#include <nnet.h>

class HandKinematics
{
public:
	HandKinematics();
	~HandKinematics();

	void learn(YARPBottle &newPoint);
	
	void query(YARPImageOf<YarpPixelMono> &in)
	{
		YARPShapeEllipse el;
		if (_armPort.Read(0))
		{
			_armPosition = _armPort.Content()._current_position;
		}
		
		_query(_armPosition, el);
		_fitter.plotEllipse(el, in);
	}

	void query(YARPShapeEllipse &el)
	{
		if (_armPort.Read(0))
		{
			_armPosition = _armPort.Content()._current_position;
		}
		_query(_armPosition, el);
	}

	YARPShapeEllipse query()
	{
		YARPShapeEllipse el;
		query(el);
		return el;
	}

private:
	void _query(const YVector &position, YARPShapeEllipse &el)
	{
		double tmp[3];
		double c[2];
		double p[3];
		tmp[0] = position(1);
		tmp[1] = position(2);
		tmp[2] = position(3);
		center.sim(tmp, c);
		parameters.sim(tmp, p);
	
		el.rho = c[0];
		el.theta = c[1];
		el.a11 = /*p[0];*/ 0.005;
		el.a12 = /*p[1];*/ 0;
		el.a22 = /*p[2];*/ 0.005;

		_npoints++;
	}

	void _dumpToDisk(const YVector &arm, const YVector &head, const YARPShapeEllipse &ellipse);
	
	YARPInputPortOf<YARPControlBoardNetworkData>  _armPort;
	YVector _armPosition;
	YVector _headPosition;
	YARPLpConicFitter _fitter;
	YARPLogFile		  _log;
	
	nnet center;
	nnet parameters;

	int _npoints;
};

#endif