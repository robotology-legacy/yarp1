#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPSciDeviceDriver.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <iostream>

const char __portName[] = "/dev/ttyS0";

using namespace std;

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

	YARPSciDeviceDriver arm;

	SciOpenParameters openPar(__portName);
	
	arm.open(&openPar);
	
	while(true)
	{
		arm.readDebugger();
	}

	return 0;
}