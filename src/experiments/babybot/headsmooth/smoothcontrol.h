#ifndef __smoothcontrolhh__
#define __smoothcontrolhh__

#include <YARPMatrix.h>
#include <YARPPidFilter.h>

class SmoothControl
{
public:
	SmoothControl(int insize, int outsize);
	~SmoothControl();

	void apply(const YVector &in, YVector &out)
	{
		out = 0.0;
	}

private:
	int _inSize;
	int _outSize;
};

#endif