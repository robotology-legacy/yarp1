#ifndef __VORCONTROLHH__
#define __VORCONTROLHH__

// implement VOR

#define VOR_CONTROL_VERBOSE

#ifdef VOR_CONTROL_VERBOSE
#define VOR_CONTROL_DEBUG(string) YARP_DEBUG("VOR_CONTRL_DEBUG:", string)
#else  VOR_CONTROL_DEBUG(string) YARP_NULL_DEBUG
#endif

#include <YARPRobotMath.h>
#include <YARPString.h>

class VorControl
{
public:
	VorControl(int nj, int ns, double init = 0.0){
		deltaQ.Resize(nj);
		k_vor.Resize(ns);

		k_vor = init;
	}

	~VorControl() {}

	void set(double gain){
		k_vor = gain;
	}

	inline YVector & handle(const YVector &in){
		deltaQ = 0.0;
		
		deltaQ(4) = _vor(in(1), k_vor(3));
		deltaQ(5) = -_vor(in(1), k_vor(3));
		deltaQ(3) = _vor(in(2), k_vor(2));
			
		return deltaQ;
	}

	void load(YARPString &path, YARPString &file)
	{

	}

	YVector deltaQ;			//resulting command
	YVector k_vor;			

private:
	inline double _vor(double input, double gain)
	{
		return input*degToRad*gain;
		/* // old code, doesn't seem to be needed...
		if (fabs(input/double(_inertial::_normGyroOut)) > _inertial::_in_thr)
			return input*degToRad*gain;
		else
			return 0;
		*/
	}

};

#endif