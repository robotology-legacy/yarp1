// remotelearn.cpp : Defines the entry point for the console application.
// -- by nat Dec. 2003

// accept the following commands (bottles)
// float variables: train samples
// YBVRLTrainNow: force the network to train on current samples
// YBVRLNEpoch + float set new value for #epoch
// YBVRLBatchSize + float set new batch size
	
#include "lbehavior.h"
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPPort.h>
#include <YARPParseParameters.h>
#include <./conf/YARPVocab.h>

const char __filename[] = "test.ini";
const char __outFilename[] = "out.ini";
const char __basePortName[] = "/remotelearn";
const int __nIterations = 100000;
const int __nSamples = -1;

#include <YARPRnd.h>

double nIterations = __nIterations;
int nSamples = __nSamples;

void parse(Learner &learner, YARPBottle &b);

int main(int argc, char* argv[])
{
	// parse input parameters and form port names
	YARPString inFilename;
	YARPString outFilename;
	inFilename.append(GetYarpRoot());
	inFilename.append("/conf/babybot/");
	outFilename = inFilename;

	YARPString basePortName, inputPort, outputPort;
	YARPString netFile, outFile, trainSetFile;
	
	if (!YARPParseParameters::parse(argc, argv, "conf", netFile))
	{
		netFile = __filename;
	}
	ACE_OS::printf("loading config  file from: %s\n", netFile.c_str());

	if (!YARPParseParameters::parse(argc, argv, "name", basePortName))
	{
		basePortName = __basePortName;
	}
	
	if (!YARPParseParameters::parse(argc, argv, "epoch", &nIterations))
	{
		nIterations = __nIterations;
	}
	ACE_OS::printf("nepoch: %lf\n", nIterations);
	
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
		ACE_OS::printf("result of training will be saved on: %s\n", filename.c_str());
	}
	
	if (YARPParseParameters::parse(argc, argv, "trainset", trainSetFile))
	{
		YARPString filename;
		filename.append(GetYarpRoot());
		filename.append("/conf/babybot/");
		filename.append(trainSetFile);
		_learner.setTrainSetFile(filename);
		ACE_OS::printf("trainset will be saved on: %s\n", filename.c_str());
	}
		
	if (!YARPParseParameters::parse(argc, argv, "batchsize", &nSamples))
	{
		nSamples = __nSamples;
		ACE_OS::printf("batchsize option not specified using default\n");
	}
	ACE_OS::printf("batchsize: %d\n", nSamples);
	

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
		_inputPort.Content().display();
		
		parse(_learner, _inputPort.Content());
	}
	return 0;
}

void parse(Learner &learner, YARPBottle &b)
{
	double dummy;
	if (b.tryReadFloat(&dummy))
	{
		// HANDLE NEW SAMPLE
		learner.add(b);

		ACE_OS::printf("Received new sample:#%d\n", learner.howMany());
		// CHECK train condition
		if (nSamples < 0)
			return;	// no training allowed
		else if (learner.howMany()%nSamples== 0)
			learner.train((int) nIterations, true);
		return;
	}

	// is it a vocab ?
	YBVocab tmp;
	if (b.tryReadVocab(tmp))
		b.moveOn();

	if (tmp == YBVRLTrainNow)
	{
		learner.train((int) nIterations, true);
		return;
	}
	else if (tmp == YBVRLNEpoch)
	{
		b.readFloat(&nIterations);
		ACE_OS::printf("#epoch: %lf\n", nIterations);
		return;
	}
	else if (tmp == YBVRLBatchSize)
	{
		b.readFloat(&dummy);
		nSamples = (int) dummy;
		ACE_OS::printf("new batch size: %d\n", nSamples);
	}
	else
	{
		ACE_OS::printf("Message not recognized\n");
	}
}