// by nat June 2003


#include <conf/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <YARPThread.h>
#include <YARPSemaphore.h>
#include <YARPScheduler.h>
#include <YARPTime.h>

#include <YARPForceSensor.h>

int ParseParams (int argc, char *argv[]) 
{


	return YARP_OK; 
}


int main (int argc, char *argv[])
{
	YARPScheduler::setHighResScheduling ();

	double force[3];
	double torque[3];

	YARPBabybotForceSensor fs;

	fs.initialize();
	
	while(true)
	{
		fs.read(force, torque);
		ACE_OS::sleep(40*1000);
	}

	fs.uninitialize();

	return YARP_OK;
}



