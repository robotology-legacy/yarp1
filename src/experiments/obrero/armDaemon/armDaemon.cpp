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
#include <yarp/YARPThread.h>
#include <yarp/YARPIterativeStats.h>
#include <yarp/YARPRateThread.h>
#include <iostream>

#include "armSampler.h"
#include "armGravity.h"

YARPInputPortOf<YARPBottle> _inputPort;

const int __samplerRate = 5;
const int MESSAGE_LABEL = 5;

using namespace std;

void _handleMsg(int msg, YARPBottle &bot, YARPArm &arm);

int main()
{
  YARPArm arm;
  ArmSampler sampler(&arm, __samplerRate);
  ArmGravity gravity(&arm);

  ACE_OS::printf("\nHello from YARP!\n\n");

  _inputPort.Register("/armDaemon/i:bot");
  sampler.Register("/armDaemon/torques/o:vect", "/armDaemon/positions/o:vect", "/armDaemon/error/o:vect");
  gravity.register1("/armDaemon/gravity/i:vect");
  gravity.register2("/armDaemon/offset/i:vect");
	
  char path[255];
  sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

  if (arm.initialize(path, "arm.ini") == YARP_OK)
    fprintf(stderr, "Init was OK\n");
  else
    {
      fprintf(stderr, "Troubles opening YARPArm\n");
      exit(-1);
    }

  sampler.start();
  gravity.Begin();
  while(1)
     {
       int msg;
       ACE_OS::printf("Waiting for a new command\n");
       _inputPort.Read();
       ACE_OS::printf("got a new bottle\n");
	  
       YARPBottle &bot = _inputPort.Content();
       bot.display();
       printf("\n");
	  
       bool ret = bot.readInt(&msg);
       if (ret && msg==MESSAGE_LABEL)
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
  LowLevelPID pid;
  bool ret;
  double tmpVector[nj];
  int tmpIntVector[nj];
  double val = 0.0;
  int ival = 0;
  int j;
  for (j=0; j<nj;j++)
    {
      tmpVector[j] = 0.0;
      tmpIntVector[j] = 0;
    }

  switch (msg)
    {
    case 0:
      arm.positionMode();
      break;
    case 1:
      ret = bot.readDoubleVector(tmpVector, nj);
      if (!ret)
	{
	  //try reading int vector
	  ret = bot.readIntVector(tmpIntVector, nj);
	  for(j=0;j<nj;j++)
	    tmpVector[j]=(double)tmpIntVector[j];
	}

      arm.setPositions(tmpVector);
      ACE_OS::printf("setting position: ");
      for(int k = 0; k<6; k++)
	ACE_OS::printf("%lf\t",tmpVector[k]);
      ACE_OS::printf("\n");
      break;
    case 2:
      bot.readInt(&j);
      ret = bot.readFloat(&val);
      if (!ret)
	{
	  ret = bot.readInt(&ival);
	  val = (double)(ival);
	}

      arm.setPositionRelative(j, val);
      ACE_OS::printf("setting relative position: ");
      ACE_OS::printf("%d\t%lf\n", j, val);
      break;
    case 3:
      bot.readInt(&j);
      ret=bot.readFloat(&val);
      if (!ret)
	{
	  ret=bot.readInt(&ival);
	  val=(double)(ival);
	}
      arm.setPosition(j, val);
      ACE_OS::printf("setting absolute position: ");
      ACE_OS::printf("%d\t%lf\n", j, val);
      break;
    case 4:
      arm.stopMotion();
      break;
    case 5:
      ret = bot.readDoubleVector(tmpVector, nj);
      if (!ret)
	{
	  //try reading int vector
	  ret = bot.readIntVector(tmpIntVector, nj);
	  for(j=0;j<nj;j++)
	    tmpVector[j]=(double)tmpIntVector[j];
	}
      arm.setPositionsRelative(tmpVector);
      break;
    case 6:
      arm.forceMode();
      break;
    case 7:
      bot.readInt(&j);
      ret = bot.readFloat(&val);
      if (!ret)
	{
	  ret = bot.readInt(&ival);
	  val = (double)(ival);
	}
      if (ret)
	arm.setOffset(j,val);
      break;
    case 8:
      ret = bot.readDoubleVector(tmpVector, nj);
      if (!ret)
	{
	  //try reading int vector
	  ret = bot.readIntVector(tmpIntVector, nj);
	  for(j=0;j<nj;j++)
	    tmpVector[j]=(double)tmpIntVector[j];
	}
      if (ret)
	arm.setOffsets(tmpVector);
      break;
    case 9:
      ret = bot.readInt(&j);
      if (ret)
	{
	  ret=bot.readInt(&ival);
	  if (ret)
	    {
	      pid.KP=(double) ival;
	      arm.setPID(j,pid);
	    }
	}
      break;
    default:
      ACE_OS::printf("Message not recognized, nothing done\n");
    }
  
}

