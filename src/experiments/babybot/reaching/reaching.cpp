// reaching.cpp : Defines the entry point for the console application.
//

#include <YARPPort.h>
#include <YARPBottle.h>
#include <YARPBottleContent.h>
#include <YARPMatrix.h>
#include <YARPVectorPortContent.h>

const char *__nnetFile1 = "reach.ini";

#define DECLARE_INPUT_PORT(TYPE, NAME, PROTOCOL) YARPInputPortOf<TYPE> NAME(YARPInputPort::DEFAULT_BUFFERS, PROTOCOL)
#define DECLARE_OUTPUT_PORT(TYPE, NAME, PROTOCOL) YARPOutputPortOf<TYPE> NAME(YARPOutputPort::DEFAULT_OUTPUTS, PROTOCOL)

void printFrameStatus(int n);

int _headFrames = 0;
int _armFrames = 0;
int _nFrames = 0;

int main(int argc, char* argv[])
{
	DECLARE_INPUT_PORT (YVector, _inPortHead, YARP_MCAST);
	DECLARE_INPUT_PORT (YVector, _inPortArm, YARP_MCAST);
	DECLARE_INPUT_PORT (YARPBottle, _inPortRemoteLearn, YARP_TCP);
	
	DECLARE_OUTPUT_PORT (YARPBottle, _outPortArmCmd, YARP_TCP);
	DECLARE_OUTPUT_PORT (YARPBottle, _outPortRemoteLearn, YARP_TCP);
	
		
	YARPBottle _outputTrainBottle;
	
	_inPortHead.Register("/reaching/head/i");
	_inPortArm.Register("/reaching/arm/i");
	_inPortRemoteLearn.Register("/reaching/nnet/i");

	_outPortArmCmd.Register("/reaching/command/o");
	_outPortRemoteLearn.Register("/reaching/nnet/o");
	
	YVector _arm(6);
	YVector _head(5);
	
	while (true)
	{
		// blocking on head port
		_inPortHead.Read();
		_head = _inPortHead.Content();
		_headFrames++;

		/////////////////////////// segmentation
		// poll arm position
		if (_inPortArm.Read(0))
		{
			_arm = _inPortArm.Content();
			_armFrames++;
		}
		
		printFrameStatus(_nFrames);
		_nFrames++;
	}
	return 0;
}

////////// print frame status
void printFrameStatus(int n)
{
	if (n%1000 == 0)
		ACE_OS::printf("HeadFrame# %d\tArmFrame# %d\n", _headFrames, _armFrames);
}