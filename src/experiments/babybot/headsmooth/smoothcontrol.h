#ifndef __smoothcontrolhh__
#define __smoothcontrolhh__

#include <YARPMatrix.h>
#include <YARPPidFilter.h>
#include <YARPString.h>

#include <YARPLogpolar.h>

class SmoothControl
{
public:
	SmoothControl(const YARPString &iniFile, int insize, int outsize);
	~SmoothControl();

	void apply(const YVector &in, YVector &out)
	{
		double x = in(1);
		double y = in(2);

		_threshold(&x, _logpolarParams::_xsizefovea/2);
		_threshold(&y, _logpolarParams::_ysizefovea/2);

		out(1) = 0.0;
		out(2) = 0.0;
		out(3) = _pids[0].pid(y);
		// out(4) = _pids[1].pid(x);
		out(5) = _pids[2].pid(x);
		out(4) = -out(5);
	}

private:
	void _threshold(double *v, double th)
	{
		if (*v > th)
			*v = th;
		else if (*v < -th)
			*v = -th;
	}

	int _inSize;
	int _outSize;
	int _nPids;
	YARPPidFilter *_pids;

	YARPString _iniFile;
	YARPString _path;
};

#endif
