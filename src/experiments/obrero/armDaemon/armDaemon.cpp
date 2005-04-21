#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>
// #include <yarp/YARPSciDeviceDriver.h>
#include <yarp/YARPRobotHardware.h>
#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <iostream>

YARPInputPortOf<YARPBottle> _inputPort;

using namespace std;

void _handleMsg(int msg, YARPBottle &bot, YARPArm &arm);

int main()
{
	ACE_OS::printf("\nHello from YARP!\n\n");

	_inputPort.Register("/armDaemon/i:bot");
	char path[255];
	sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

	YARPArm arm;

	if (arm.initialize(path, "arm.ini") == YARP_OK)
		printf("Init was OK");
	else
		printf("Init was NOT OK");

	ACE_OS::printf("Entering loop\n");
	while(1)
	{
	  int msg;
	  _inputPort.Read();
	  ACE_OS::printf("got a new bottle\n");
	  
	  YARPBottle &bot = _inputPort.Content();
	  bot.display();
	  
	  bool ret = bot.readInt(&msg);
	  if (ret && msg==5)
	    {
	      bot.readInt(&msg);
	      _handleMsg(msg, bot, arm);
	    }
	  else
	    ACE_OS::printf("Warning the message is not for me, skipping it\n");

	}
}

void _handleMsg(int msg, YARPBottle &bot, YARPArm &arm)
{
  int nj = arm.nj();
  double tmpVector[nj];
  double val = 0.0;
  int j;
  for (j=0; j<nj;j++)
    tmpVector[j] = 0.0;

  switch (msg)
    {
    case 0:
      arm.activatePID();
      break;
    case 1:
      bot.readDoubleVector(tmpVector, nj);
      // LATER: we want to use the setPositions functions
      // however we dont want to move the wrist yet.
      //arm.setPositions(tmpVector);
      arm.setPosition(0, tmpVector[0]);
      arm.setPosition(1, tmpVector[1]);
      arm.setPosition(2, tmpVector[2]);
      arm.setPosition(3, tmpVector[3]);
      ACE_OS::printf("setting position: ");
      for(int k = 0; k<6; k++)
	ACE_OS::printf("%lf\t",tmpVector[k]);
      ACE_OS::printf("\n");
      break;
    case 2:
      bot.readInt(&j);
      bot.readFloat(&val);
      arm.setPositionRelative(j, val);
      ACE_OS::printf("setting relative position: ");
      ACE_OS::printf("%d\t%lf\n", j, val);
      break;
    case 3:
      bot.readInt(&j);
      bot.readFloat(&val);
      arm.setPosition(j, val);
      ACE_OS::printf("setting absolute position: ");
      ACE_OS::printf("%d\t%lf\n", j, val);
      break;
    case 4:
      arm.stopMotion();
      break;
    default:
      ACE_OS::printf("Message not recognized, nothing done\n");
    }
  
}
