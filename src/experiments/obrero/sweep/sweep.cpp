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
#include <yarp/YARPThread.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <stdio.h>

// #define NO_ARM

const char __portName[] = "/dev/ttyS0";
void setPositions(YARPArm &arm, YVector &v);

using namespace std;

const int __nj = 6;
typedef list<YVector> myList;

void _parseFile(char *file, myList &list, YVector &home);

YARPInputPortOf<YARPBottle> _inPort;


class ArmSampler: public YARPRateThread
{
public:
	ArmSampler(YARPArm *p, int rate): YARPRateThread("ArmSamplerThread", rate)
	{
		arm=p;
		// outputPort.Register("/arm/o:bot");
	}
	void doInit()
	{
		torques.Resize(__nj);
		positions.Resize(__nj);
		accs.Resize(__nj);
		torques = 0.0;
		positions = 0.0;
		accs = 0.0;

		dumpFile = fopen("dump.txt", "wt");
	}

	void doLoop()
	{
		int ret1,ret2,ret3;
		ret1 = arm->getPositions(positions.data());
		// ret2 = arm->getTorques(torques.data());
		
		// ret3 = arm->getIstAccs(accs.data());
		// accs = accs*(500*500);
		// ret1 = arm->getIstAccs(positions.data());
		// positions = positions*500*500;
		// ret2 = arm->getVelocities(accs.data());
		// accs = accs*250;

		/*
		if ( (ret1==YARP_OK)&&(ret2==YARP_OK) )
		{
			for(int i = 1; i <= 6; i++)
				fprintf(dumpFile, "%lf\t", positions(i));
		
			for(int i = 1; i <= 6; i++)
				fprintf(dumpFile, "%lf\t", torques(i));
			
			for(int i = 1; i <= 6; i++)
				fprintf(dumpFile, "%lf\t", accs(i));

			fprintf(dumpFile,"\n");
		}
		else
		{
			printf("Failed to read accelerations\n");
		}*/

	//	bottle.writeInt(2);	//I'm 2
	//	bottle.writeFloat(torques(3));
	//	outputPort.Content() = bottle;
	//	outputPort.Write(0); //don't block
	//	bottle.reset();
	}

	void doRelease()
	{
		fclose(dumpFile);
	}

private:
	YARPArm *arm;
	YVector torques;
	YVector positions;
	YVector accs;
	FILE *dumpFile;
	YARPOutputPortOf<YARPBottle> outputPort;
	YARPBottle bottle;
};

class ArmController: public YARPRateThread
{
public:
	ArmController(YARPArm *p,int rate): YARPRateThread("ArmControllerThread", rate)
	{
		arm = p;
		readyF = false;
		_inhibited = true;
	}

	int load(char *path)
	{
		_parseFile(path, posList);
	}

	void doInit()
	{
		index = posList.begin();
		end = posList.end();
		_inhibited = false;
	}
	
	void inhibit()
	{ _inhibited = true; }

	void doLoop()
	{
		if (_inhibited)
		{
			printf("ArmController inhibited\n");
			return;
		}

		if(index==end)
		{
			index = posList.begin();
		}
		
		_setPositions((*index));
		index++;
	}

	void doRelease(){}

	void ready()
	{
		if (readyF)
			return;
		_setPositions(startPosition);
		YARPTime::DelayInSeconds(3.0);
		readyF = true;
	}
	
	// overload
	virtual void start(bool wait=true)
	{
		if(!readyF)
		{
			printf("Sorry arm not ready\n");
			return;
		}
		if (isRunning())
		{
			printf("Controller already running\n");
			if (_inhibited) // wake up arm, first put it in ready position
			{
				ready();
				_inhibited = false;
			}
		}
		else
		{
			printf("Starting arm controller\n");
			YARPRateThread::start(true);
		}

	}
	
	void stopArm()
	{
		if (_inhibited)
			return;

		inhibit();
		readyF=false;
		arm->stopMotion();
	}

	void park()
	{
		stopArm();
		readyF = false;
		printf("Parking arm...");
		_setPositions(startPosition);
		YARPTime::DelayInSeconds(4.0);
		_setPositions(homePosition);
		YARPTime::DelayInSeconds(3.0);
		printf("done!\n");
	}

private:
	void _setPositions(YVector &v)
	{
	#ifndef NO_ARM
		for(int i = 1; i<=__nj; i++)
				printf("%lf\t", v(i));
		printf("\n");

		arm->setPosition(0,v(1));
		arm->setPosition(1,v(2));
		arm->setPosition(2,v(3));
		arm->setPosition(3,v(4));
		
	#endif
	}

	void _parseFile(char *path, myList &list)
	{
		YARPConfigFile cfg(path, "sweep.cfg");
		printf("%s\n", path);

		int n = __nj;
		int length;
		homePosition.Resize(__nj);
		startPosition.Resize(__nj);
	
		if (cfg.get("[POS]", "N=", &length)==YARP_FAIL)
			printf("Error reading file\n");
		cfg.get("[POS]", "Home=", homePosition.data(),__nj);
		cfg.get("[POS]", "Ready=", startPosition.data(),__nj);

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
			printf("%lf\t", homePosition(i));
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

		printf("Starting position:\n");
		for(int i = 1; i<=__nj; i++)
			printf("%lf\t", startPosition(i));
		printf("\n");
	}

	YARPArm *arm;

	YVector homePosition;
	YVector startPosition;
	myList posList;
	myList::iterator index;
	myList::iterator end;

	bool readyF;
	bool _inhibited;
};

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

//	set_yarp_debug(100,100);

	_inPort.Register("/arm/i:bot");

	// read positions from file
	char path[255];
	sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

	YARPArm arm;
	ArmSampler sampler(&arm, 100);
	ArmController controller(&arm,3000);
	controller.load(path);
	
	if (arm.initialize(path, "arm.ini") == YARP_OK)
		printf("Init was OK");
	else
	{
		printf("Init was NOT OK");
		exit(-1);
	}

	sampler.start();
	arm.activatePID();
	YARPTime::DelayInSeconds(2.0);

	// controller.ready();

	printf("Listening...\n");

	bool exit=false;
	char c;
	c='\0';
	while(!exit)
	{
		_inPort.Read();

		// printf("---------->> Received a bottle: \n");
		// _inPort.Content().display();

		YARPBottle tmp;
		tmp = _inPort.Content();
		
		int iTmp;
		if(tmp.readInt(&iTmp))
		{
			if (iTmp==3)
			{	
				if (tmp.readInt(&iTmp))
				{
					if (iTmp==2)
					{
						// stop arm
						controller.stopArm();
					}
					else if (iTmp==1)
					{
						// start arm
						controller.ready();
						controller.start();	// wait
					}
					else if (iTmp==3)
					{
						controller.stopArm();
						controller.ready();
						
					}
					else if (iTmp==4)
					{
						controller.park();
					}
				}
				else
				{
					printf("Sorry message not known\n");
				}
			}
			else
			{
				exit = true;
			}
		}
		else
		{
			exit = true;
		}
	}
	
	controller.terminate(0);	// no timeout
	controller.park();
	
	sampler.terminate();

	ACE_OS::printf("\ndone!\n");
	arm.uninitialize();
	
	return 0;
}
