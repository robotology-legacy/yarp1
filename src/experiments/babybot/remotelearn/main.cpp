// remotelearn.cpp : Defines the entry point for the console application.
//

#include "lbehavior.h"
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPPort.h>
#include <YARPParseParameters.h>

const char __filename[] = "test.ini";
const char __outFilename[] = "out.ini";
const char __basePortName[] = "/remotelearn";
const int __nIterations = 100000;
const int __nSamples = 50;

#include <YARPRnd.h>
int main(int argc, char* argv[])
{
	// parse input parameters and form port names
	YARPString inFilename;
	YARPString outFilename;
	inFilename.append(GetYarpRoot());
	inFilename.append("/conf/babybot/");
	outFilename = inFilename;

	double nIterations;
	YARPString basePortName, inputPort, outputPort;
	YARPString netFile, outFile, trainSetFile;
	
	if (!YARPParseParameters::parse(argc, argv, "conf", netFile))
	{
		netFile = __filename;
	}
	if (!YARPParseParameters::parse(argc, argv, "name", basePortName))
	{
		basePortName = __basePortName;
	}
	
	if (!YARPParseParameters::parse(argc, argv, "epoch", &nIterations))
	{
		nIterations = __nIterations;
	}
	
	inFilename.append(netFile);
	outFilename.append(outFile);

	inputPort = basePortName;
	outputPort = basePortName;
	inputPort.append("/i");
	outputPort.append("/o");
	/////////////////////////////////////////////
	
	Learner _learner(inFilename);

	if (YARPParseParameters::parse(argc, argv, "outnet", outFile))
	{
		YARPString filename;
		filename.append(GetYarpRoot());
		filename.append("/conf/babybot/");
		filename.append(outFile);
		_learner.setOutConfigFile(filename);
	}

	if (YARPParseParameters::parse(argc, argv, "trainset", trainSetFile))
	{
		YARPString filename;
		filename.append(GetYarpRoot());
		filename.append("/conf/babybot/");
		filename.append(trainSetFile);
		_learner.setTrainSetFile(filename);
	}
	
	int nSamples = __nSamples;
	if (!YARPParseParameters::parse(argc, argv, "batchSize", &nSamples))
	{
		nSamples = __nSamples;
	}

	/////////////////////////////////////////////
	
	YARPInputPortOf<YARPBottle> _inputPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	YARPOutputPortOf<YARPBottle> _outputPort(YARPInputPort::DEFAULT_BUFFERS, YARP_TCP);
	_inputPort.Register(inputPort.c_str());
	_outputPort.Register(outputPort.c_str());

	YARPBottle outputBottle;
	_learner.setOutputPort(&_outputPort, &outputBottle);

	while(true)
	{
		_inputPort.Read();
		_learner.add(_inputPort.Content());
		// _inputPort.Content().display();
		
		ACE_OS::printf("Received new sample:#%d\n", _learner.howMany());

		if (_learner.howMany()%nSamples== 0)
			_learner.train((int) nIterations, true);
	}
	return 0;
}
