#include "armmap.h"

ArmMap::ArmMap(const char *nnetFile)
{
	char *root = GetYarpRoot();
	char filename1[256];
	
	// load nnets
	ACE_OS::sprintf (filename1, "%s/conf/babybot/%s", root, nnetFile);
	if (_nnet.load(filename1)!=YARP_OK)
	{
		ACE_OS::printf("Error, cannot read neural network file %s", filename1);
		exit(-1);
	}

	_arm.Resize(__nJointsArm);
	_arm = 0.0;
}

ArmMap::~ArmMap()
{

}