// remotelearn.cpp : Defines the entry point for the console application.
//

#include "lbehavior.h"
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPPort.h>

const char __filename[] = "test.ini";
const char __inputPort[] = "/remotelearn/i";
const char __outputPort[] = "/remotelearn/o";
const int __nIterations = 10000;

int main(int argc, char* argv[])
{
	YARPString filename;
	filename.append(GetYarpRoot());
	filename.append("/conf/babybot/");
	filename.append(__filename);
	
	Learner _learner(filename);
	
	YARPInputPortOf<YARPBottle> _inputPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPOutputPortOf<YARPBottle> _outputPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	_inputPort.Register(__inputPort);
	_outputPort.Register(__outputPort);

	YARPBottle outputBottle;
	_learner.setOutputPort(&_outputPort, &outputBottle);

	while(true)
	{
		_inputPort.Read();
		_learner.add(_inputPort.Content());
		// _inputPort.Content().display();
		
		ACE_OS::printf("Received new sample:#%d\n", _learner.howMany());

		if (_learner.howMany()%60 == 0)
			_learner.train(__nIterations, true);
	}

	return 0;
}
