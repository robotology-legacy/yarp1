// vor.cpp : Defines the entry point for the console application.
//

#include <YARPScheduler.h>
#include <YARPRobotMath.h>
#include <YARPPort.h>
#include <YARPVectorPortContent.h>

#include "VorControl.h"

const int __headNj = 5;
const int __inertialSensors = 3;
const double __vorGain = 2.0;

int main(int argc, char* argv[])
{
	YARPInputPortOf<YVector> _inertialPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YVector> _vorPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	_inertialPort.Register("/vor/inertial/i");
	_vorPort.Register("/vor/o");

	VorControl _vor(__headNj, __inertialSensors, __vorGain);

	while(true)
	{
		_inertialPort.Read();
		_vorPort.Content() = _vor.handle(_inertialPort.Content());
		_vorPort.Write();
	}

	return 0;
}
