// headsmooth.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>
#include <YARPString.h>

#include "smoothcontrol.h"

const int __inSize = 2;
const int __outSize = 5;
const char *__baseName = "/headsmooth/";

int main(int argc, char* argv[])
{
	YVector _in(__inSize);
	YVector _out(__outSize);
	YARPInputPortOf<YVector> _inPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YVector> _outPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);
	SmoothControl _control;

	YARPString base(__baseName);
	_inPort.Register(base.append("i").c_str());
	_outPort.Register(base.append("o").c_str());

	while(true)
	{
		_inPort.Read();
		_control.apply(_inPort.Content(), _outPort.Content());
		_outPort.Write();
	}

	return 0;
}
