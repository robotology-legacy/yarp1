// vor.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

#include "../HeadControl/VorControl.h"

int main(int argc, char* argv[])
{
	YARPInputPortOf<YVector> _inertialPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YVector> _vorPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	_inertialPort.Register("/vor/inertial/i");
	_vorPort.Register("/vor/o");

	VorControl _vor(6,3,2);

	while(true)
	{
		_inertialPort.Read();
		_vorPort.Content() = _vor.handle(_inertialPort.Content());
		_vorPort.Write();
	}

	return 0;
}
