// remotelearn.cpp : Defines the entry point for the console application.
//

#include "lbehavior.h"
#include <YARPBottle.h>
#include <YARPPort.h>

const char __filename[] = "test.ini";
const char __inputPort[] = "/nnet/i";
const int __nIterations = 100000;

int main(int argc, char* argv[])
{
	YARPString filename;
	filename.append(GetYarpRoot());
	filename.append("/conf/babybot/");
	filename.append(__filename);
	
	Learner _learner(filename);

	YARPInputPortOf<YARPBottle> _inputPort(YARPInputPort::DEFAULT_BUFFERS, YARP_UDP);
	_inputPort.Register(__inputPort);


	while(true)
	{
		_inputPort.Read();
		_learner.add(_inputPort.Content());

		ACE_OS::printf("Received new sample:#%d\n", _learner.howMany());

		if (_learner.howMany()%10 == 0)
			_learner.train(__nIterations, true);
	}

	return 0;
}
