// vor.cpp : Defines the entry point for the console application.
//

#include <yarp/YARPConfig.h>
#include <ace/config.h>

#include <yarp/YARPScheduler.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBabyBottle.h>

#include "VorControl.h"
#include "../headsink/sinkconstants.h"

const int __headNj = 5;
const int __inertialSensors = 3;
const double __vorGain = 2.0;

int main(int argc, char* argv[])
{
	YARPInputPortOf<YVector> _inertialPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	YARPOutputPortOf<YARPBabyBottle> _vorPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_UDP);

	_inertialPort.Register("/vor/inertial/i");
	_vorPort.Register("/vor/o");

	VorControl _vor(__headNj, __inertialSensors, __vorGain);

	while(true)
	{
		_inertialPort.Read();
		_vorPort.Content().reset();
		_vorPort.Content().writeYVector(_vor.handle(_inertialPort.Content()));
		_vorPort.Content().writeInt(SINK_INHIBIT_NONE);

	//	_vorPort.Content().display();
		_vorPort.Write();
	}

	return 0;
}
