#ifndef __smoothcontrolhh__
#define __smoothcontrolhh__

#include <YARPMatrix.h>

class SmoothControl
{
public:
	SmoothControl();
	~SmoothControl();

	void apply(const YVector &in, YVector &out)
	{
		out = 0.0;
	}

};

#endif