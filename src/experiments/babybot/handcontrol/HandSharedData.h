#ifndef __HAND_SHARED_DATA
#define __HAND_SHARED_DATA

#include <YARPBabybothand.h>
#include <YARPPort.h>

class HandSharedData: public YARPBabybotHand
{
public:
	HandSharedData():
	_behaviorsPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP)
	{
		_behaviorsPort.Register("/behaviors/o:2");
	}
	YARPOutputPortOf<int [2]> _behaviorsPort;
};

#endif