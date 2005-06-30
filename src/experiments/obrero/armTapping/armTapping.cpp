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
#include <yarp/YARPSemaphore.h>

#include <stdio.h>


//void setPositions(YARPArm &arm, YVector &v);

using namespace std;

const int __nj = 6;
typedef list<YVector> myList;

void _parseFile(char *file, myList &list, YVector &home);

YARPInputPortOf<YARPBottle> _inPort;
class ArmDaemon : public  YARPOutputPortOf<YARPBottle>
{
public:
  ArmDaemon(){};
  ~ArmDaemon(){};
  
  void activatePID()
  {
    /*
    _mutex.Wait();
    Content().writeInt(5);	// arm daemon code
    Content().writeInt(0);	// activate PID command
    Write(0);	// send
    _mutex.Post();
    */
  }

  void stopMotion()
  {
    _mutex.Wait();
    Content().writeInt(5);
    Content().writeInt(4);
    Write(0);
    _mutex.Post();
  }

  void setPositions(int nj, double *v)
  {
    _mutex.Wait();
    Content().writeInt(5);
    Content().writeInt(3);
    Content().writeDoubleVector(v,__nj);
    Write(0);
    _mutex.Post();
  }

  YARPSemaphore _mutex;
};

class ArmController: public YARPRateThread
{
public:
  ArmController(ArmDaemon *p,int rate): YARPRateThread("ArmControllerThread", rate)
  {
    arm = p;
    _inhibited = false;
  }

  int load(char *path, char *name)
  {
    return _parseFile(path, name);
  }

  void doInit()
  {
    _current=0;
  }
	
  void inhibit()
  { _inhibited = true; }

  void doLoop()
  {
    if (_inhibited)
      return;

    _setPositions(_positions[_current]);
    _current++;
    if(_current==2)
      _current=0;
  }

  void start()
  {
    if (YARPRateThread::isRunning())
      _inhibited=false;
    else
      YARPRateThread::start();
  }

  void doRelease(){}

  void stopArm()
  {
    if (_inhibited)
      return;

    inhibit();
    
    arm->stopMotion();
  }

private:
  void _setPositions(YVector &v)
  {
    arm->setPositions(__nj, v.data());
  }

  int _parseFile(char *path, char *file)
  {
    YARPConfigFile cfg(path, file);

    _positions[0].Resize(__nj);
    _positions[1].Resize(__nj);

    int ret;
    ret=cfg.get("[COMMANDS]", "Back=", _positions[0].data(), __nj);
    ret=cfg.get("[COMMANDS]", "Forth=", _positions[1].data(), __nj);
    
    return ret;
  }

  ArmDaemon *arm;
  
  YVector *_positions;

  bool _inhibited;
  int _current;
};

int main()
{
  ACE_OS::printf("\nHello from YARP!\n\n");

  _inPort.Register("/armTapping/i:bot");
  // read positions from file
  char path[255];
  sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

  ArmDaemon arm;
  ArmController controller(&arm,3000);

  if (controller.load(path, "armTapping.cfg")!=YARP_OK)
    {
      fprintf(stderr,"Error opening %s aborting...\n", "armTapping.cfg");
      exit(-1);
    }
	
  arm.Register("/armTapping/o:bot");
	
  printf("Listening...\n");

  bool exit=false;
  while(!exit)
    {
      _inPort.Read();

      YARPBottle & tmp = _inPort.Content();
		
      int iTmp;
      if(tmp.readInt(&iTmp))
	{
	  if (iTmp==8)
	    {	
	      if (tmp.readInt(&iTmp))
		{
		  //printf("Message ID %d\n", iTmp);
		  if (iTmp==3)
		    {
		      // stop arm
		      controller.stopArm();
		    }
		  else if (iTmp==1)
		    {
		      // start arm
		      controller.start();
		    }
		}
	      else
		{
		  fprintf(stderr, "ArmTapping: Sorry message not known\n");
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
	
  ACE_OS::printf("\ndone!\n");
	
  return 0;
}
