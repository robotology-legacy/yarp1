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
#include <yarp/YARPRateThread.h>
#include <yarp/YARPIterativeStats.h>
#include <iostream>

YARPInputPortOf<YARPBottle> _inputPort;

const int __samplerRate = 30;
const int MESSAGE_LABEL = 5;

using namespace std;

void _handleMsg(int msg, YARPBottle &bot, YARPArm &arm);

class ArmSampler: public YARPRateThread
{
public:
  ArmSampler(YARPArm *p, int rate);
  ~ArmSampler();
  void  doInit();
  void doRelease();
  void  doLoop();
  void Register(const char *tName, const char *pName);

private:
  int _nj;
  YARPArm *arm;
  YVector positions;
  YVector torques;
  YARPOutputPortOf<YVector> positionsPort;
  YARPOutputPortOf<YVector> torquesPort;
  FILE *dumpFile;
  IterativeStats _stats;
};

int main()
{
  YARPArm arm;
  ArmSampler sampler(&arm, __samplerRate);

  ACE_OS::printf("\nHello from YARP!\n\n");

  _inputPort.Register("/armDaemon/i:bot");
  sampler.Register("/armDaemon/torques/o:vect", "/armDaemon/positions/o:vect");
	
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
  while(1)
     {
       int msg;
       ACE_OS::printf("Waiting for a new command\n");
       _inputPort.Read();
       ACE_OS::printf("got a new bottle\n");
	  
       YARPBottle &bot = _inputPort.Content();
       bot.display();
	  
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
      arm.activatePID();
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
    default:
      ACE_OS::printf("Message not recognized, nothing done\n");
    }
  
}

///////////////// ArmSampler class
ArmSampler::ArmSampler(YARPArm *p, int rate): YARPRateThread("ArmSamplerThread", rate),
					      positionsPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST),
					      torquesPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
{
  arm=p;
  _nj = arm->nj();
  positions.Resize(_nj);
  torques.Resize(_nj);
  positions = 0.0;
  torques = 0.0;
  dumpFile = NULL;
}

ArmSampler::~ArmSampler()
{

}

void ArmSampler::Register(const char *t, const char *p)
{
  torquesPort.Register(t);
  positionsPort.Register(p);
}

void ArmSampler::doInit()
{
  dumpFile = fopen("dump.txt", "wt");
}

void ArmSampler::doLoop()
{
  int ret1,ret2,ret3;

  double t1=YARPTime::GetTimeAsSeconds();
  ret1 = arm->getPositions(positions.data());
  ACE_ASSERT(ret1==YARP_OK);
  ret2 = arm->getTorques(torques.data());
  ACE_ASSERT(ret2==YARP_OK);
  double t2=YARPTime::GetTimeAsSeconds();
  
  _stats+=(t2-t1);
#if 0
  if (_stats.elem()==100)
    {
      fprintf(stderr, "Time: %lf +/- %lf\n", _stats.get_mean(), _stats.get_std());
      _stats.reset();
    }
#endif
  torquesPort.Content()=torques;
  positionsPort.Content()=positions;
  
#if 0
  //  fprintf(stderr, ".");
 for(int k = 0; k<4; k++)
    printf("%lf\t", positions(k+1));
  printf("\n");
#endif
  //  if (dumpFile!=NULL)
  //    {
      //      for(int k=0;k<_nj;k++)
      //	fprintf(dumpFile, "%lf\t", positions[k]);
      //      for(int k=0;k<_nj;k++)
      //	fprintf(dumpFile, "%lf\t", torques[k]);

      //      printf("%lf\n", positions[3]);

  //  fprintf(dumpFile, "\n");
  //    }

  torquesPort.Write();
  positionsPort.Write();
}

void ArmSampler::doRelease()
{
  fclose(dumpFile);
}

