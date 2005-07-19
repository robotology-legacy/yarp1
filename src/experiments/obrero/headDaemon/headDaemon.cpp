#include <yarp/YARPConfig.h>
#include <ace/config.h>
#include <ace/OS.h>

#include <yarp/YARPSemaphore.h>
#include <yarp/YARPTime.h>
#include <yarp/YARPRobotMath.h>
#include <yarp/YARPPort.h>
#include <yarp/YARPBottle.h>
#include <yarp/YARPRateThread.h>
#include <iostream>
#include <ObreroHead.h>

YARPInputPortOf<YARPBottle> _inputPort;

const int __samplerRate = 50;
const int MESSAGE_LABEL = 6;

//#define __HEAD_SAMPLER_VERBOSE__

using namespace std;

void _handleMsg(int msg, YARPBottle &bot, ObreroHead &head);

class HeadSampler: public YARPRateThread
{
public:
  HeadSampler(ObreroHead *p, int rate);
  ~HeadSampler();
  void  doInit();
  void doRelease();
  void  doLoop();
  void Register(const char *pName);

private:
  int _nj;
  ObreroHead *head;
  YVector positions;

  YARPOutputPortOf<YVector> positionsPort;

  FILE *dumpFile;
};

int main()
{
  ObreroHead head;
  head.initialize();
  HeadSampler sampler(&head, __samplerRate);

  ACE_OS::printf("\nHello from YARP!\n\n");

  _inputPort.Register("/headDaemon/i:bot");
  sampler.Register("/headDaemon/positions/o:vect");
	
  sampler.start();
  ACE_OS::printf("Entering loop\n");
  while(1)
    {
      int msg;
      _inputPort.Read();
      ACE_OS::printf("got a new bottle\n");
	  
      YARPBottle &bot = _inputPort.Content();
      bot.display();
	  
      bool ret = bot.readInt(&msg);
      if (ret && msg==MESSAGE_LABEL)
	{
	  bot.readInt(&msg);
	  _handleMsg(msg, bot, head);
	}
      else
	ACE_OS::printf("Warning the message is not for me, skipping it\n");

    }
}

void _handleMsg(int msg, YARPBottle &bot, ObreroHead &head)
{
  int nj = head.nj();
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
      //      arm.activatePID();
      ACE_OS::printf("ActivatePID, command not supported\n");
      break;
    case 1:
      ret = bot.readDoubleVector(tmpVector, nj);
      if (!ret)
	{
	  //try reading int vector
	  ret = bot.readIntVector(tmpIntVector, nj);
	  if (!ret)
	    break;
	  for(j=0;j<nj;j++)
	    tmpVector[j]=(double)tmpIntVector[j];
	}

      head.setPositions(tmpVector);

      ACE_OS::printf("setting position: ");
      for(int k = 0; k<nj; k++)
	ACE_OS::printf("%lf\t",tmpVector[k]);
      ACE_OS::printf("\n");
      break;
    case 2:
      ret = bot.readInt(&j);
      ret = bot.readFloat(&val);
      if (!ret)
	{
	  ret = bot.readInt(&ival);
	  if (!ret)
	    break;
	  val = (double)(ival);
	}
      head.setPositionRelative(j, val);
      ACE_OS::printf("Move joint %d of %lf (relative motion)\n", j, val);
      break;
    case 3:
      ret = bot.readInt(&j);
      ret = bot.readFloat(&val);
      if (!ret)
        {
          ret = bot.readInt(&ival);
	  if (ret)
	    val = (double)(ival);
	  else
	    break;
        }
      head.setPosition(j, val);
      ACE_OS::printf("Move joint %d of %lf (relative motion)\n", j, val);
      break;
    case 4:
      ACE_OS::printf("Stop motion ");
      ACE_OS::printf("NOT IMPLEMENTED YET\n");
      //      arm.stopMotion();
      break;
    case 5:
      ret = bot.readDoubleVector(tmpVector, nj);
      if (!ret)
	{
	  //try reading int vector
	  ret = bot.readIntVector(tmpIntVector, nj);
	  if (!ret)
	    break;
	  for(j=0;j<nj;j++)
	    tmpVector[j]=(double)tmpIntVector[j];
	}

      head.setPositionsRelative(tmpVector);

      ACE_OS::printf("setting relative position: ");
      for(int k = 0; k<nj; k++)
	ACE_OS::printf("%lf\t",tmpVector[k]);
      ACE_OS::printf("\n");
      break;
    default:
      ACE_OS::printf("Message not recognized, nothing done\n");
    }
}

///////////////// HeadSampler class
HeadSampler::HeadSampler(ObreroHead *p, int rate): YARPRateThread("HeadSamplerThread", rate),
					      positionsPort(YARPOutputPort::DEFAULT_OUTPUTS, YARP_MCAST)
{
  head=p;
  _nj = head->nj();
  positions.Resize(_nj);
  positions = 0.0;
  dumpFile = NULL;
}

HeadSampler::~HeadSampler()
{

}

void HeadSampler::Register(const char *p)
{
  positionsPort.Register(p);
}

void HeadSampler::doInit()
{
  //  dumpFile = fopen("dump.txt", "wt");
}

void HeadSampler::doLoop()
{
  int ret1,ret2,ret3;
  ret1 = head->getPositions(positions.data());
  
  positionsPort.Content()=positions;

  #ifdef __HEAD_SAMPLER_VERBOSE__
  static int count=0;
  count++;
  if (count%10==0)
    {
      count = 0;
      int x = positions[0];
      int y = positions[1];
      printf("current pos: %d %d\n", x, y);
    }
  #endif

  if (dumpFile!=NULL)
    {
      for(int k=0;k<_nj;k++)
	fprintf(dumpFile, "%lf\t", positions[k]);
      fprintf(dumpFile, "\n");
    }

  positionsPort.Write();
}

void HeadSampler::doRelease()
{
  fclose(dumpFile);
}

