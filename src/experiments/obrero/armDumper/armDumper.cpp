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

using namespace std;

YARPInputPortOf<YVector> _inPortTorque;
YARPInputPortOf<YVector> _inPortPosition;

const char filename[]="dump.txt";

void _connect(YARPPort &port, const char *outName, const char *inName)
{
  int ret = YARP_FAIL;
  char tmp[255];
  sprintf(tmp, "!%s", inName);
  port.Connect(outName, tmp);  //disconnect first
  while(ret==YARP_FAIL)
    {
      fprintf(stderr,"Trying to connect %s to %s\n", outName, inName);
      ret = port.Connect(outName, inName);
      YARPTime::DelayInSeconds(1.0);
    }
  fprintf(stderr, "ok\n");
}

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

	_inPortTorque.Register("/armDumper/torques/i:vect");
	_inPortPosition.Register("/armDumper/positions/i:vect");

	_connect(_inPortTorque, "/armDaemon/torques/o:vect", "/armDumper/torques/i:vect");
	_connect(_inPortPosition, "/armDaemon/positions/o:vect", "/armDumper/positions/i:vect");

	FILE *fp=fopen(filename, "w");
	if (fp==NULL)
	  {
	    printf("ERROR opening file\n");
	    exit(-1);
	  }

	int counter = 0;
	while(true)
	  {
	    _inPortTorque.Read();
	    _inPortPosition.Read();

	    for(int k=1; k<=4;k++)
	      fprintf(fp, "%lf\t", _inPortPosition.Content()(k));

	    for(int k=1; k<=4;k++)
	      fprintf(fp, "%lf\t", _inPortTorque.Content()(k));

	    fprintf(fp,"\n");
	    counter++;
	    if (counter%500==0)
	      {
		fprintf(stderr, "dumping...\n");
		fclose(fp);
		fp=fopen(filename, "a");
	      }
	  }
}
