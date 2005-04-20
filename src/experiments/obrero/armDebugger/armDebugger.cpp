#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <iostream>
#include <yarp/YARPRobotHardware.h>

const char __portName[] = "/dev/ttyS0";

using namespace std;

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

	char path[255];
	sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

	YARPArm arm;
	
	if (arm.initialize(path, "arm.ini") == YARP_OK)
		printf("Init was OK");
	else
	{
		printf("Init was NOT OK");
		exit(-1);
	}

	while(true)
	{
		arm._adapter.readDebugger();
	}

	return 0;
}