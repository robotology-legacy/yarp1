#include <yarp/YARPConfig.h>
#include <yarp/YARPConfigRobot.h>
#include <ace/config.h>
#include <ace/OS.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPConfigFile.h>
#include <list>
#include <yarp/YARPMatrix.h>
#include <iostream>
#include <yarp/YARPRateThread.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <stdio.h>

// #define NO_ARM

const char __portName[] = "/dev/ttyS0";
void setPositions(YARPArm &arm, YVector &v);

using namespace std;

const int __nj = 6;

char path[255];
YVector home;
typedef list<YVector> myList;
myList posList;

void _parseFile(char *file, myList &list, YVector &home);

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

	char path[255];
	sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

	// read positions from file
	_parseFile(path, posList, home);
	
	YARPArm arm;

	if (arm.initialize(path, "arm.ini") == YARP_OK)
		printf("Init was OK");
	else
	{
		printf("Init was NOT OK");
		exit(-1);
	}

	arm.activatePID();
	
	// go to home
	ACE_OS::printf("\nGoing home...\n");
 	setPositions(arm, home);
	YARPTime::DelayInSeconds(3.0);

	myList::iterator it,end;
	it = posList.begin();
	end = posList.end();
	while(it!=end)
	{
		setPositions(arm, (*it));
		it++;

		YARPTime::DelayInSeconds(3.0);
	}

	ACE_OS::printf("\nGoing back home");
 	setPositions(arm, home);
	YARPTime::DelayInSeconds(3.0);
	
	ACE_OS::printf("\ndone!\n");
	arm.uninitialize();
	
	return 0;
}

void setPositions(YARPArm &arm, YVector &v)
{
	for(int i = 1; i<=__nj; i++)
			printf("%lf\t", v(i));
	printf("\n");

	arm.setPosition(0,v(1));
	arm.setPosition(1,v(2));
	arm.setPosition(2,v(3));
	arm.setPosition(3,v(4));
	
	printf("\n");
}

void _parseFile(char *path, myList &list, YVector &home)
{
	YARPConfigFile cfg(path, "demo.cfg");
	printf("%s\n", path);

	int n = __nj;
	int length;
	home.Resize(__nj);
	
	if (cfg.get("[POS]", "N=", &length)==YARP_FAIL)
		printf("Error reading file\n");
	cfg.get("[POS]", "Home=", home.data(),__nj);
	
	YVector tmp(__nj);
	for(int i = 1; i <= length; i++)
	{
		char k[80];
		sprintf(k, "WayPoint%d=", i);
		cfg.get("[POS]", k, tmp.data(),__nj);
		list.push_back(tmp);
	}

	/// BEGIN DEBUG
	printf("N=%d\n", length);
	printf("Home: ");
	for(int i = 1; i<=__nj; i++)
		printf("%lf\t", home(i));
	printf("\n");

	myList::iterator it,end;
	it = list.begin();
	end = list.end();
	while(it!=end)
	{
		for(int i = 1; i<=__nj; i++)
			printf("%lf\t", (*it)(i));
		printf("\n");
		it++;
	}

}
