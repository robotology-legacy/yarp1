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

  void setPositions(int n, double *v)
  {
    _mutex.Wait();
    Content().writeInt(5);
    Content().writeInt(5);
    Content().writeDoubleVector(v,n);
    Write(0);
    _mutex.Post();
  }
  void setPosition(int j, double v)
  {
    _mutex.Wait();
    Content().writeInt(5);
    Content().writeInt(2);
    // I need to change this soon
    Content().writeInt(j);
    Content().writeInt((int)v);
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
    _parseFile(path, name);
  }

  void doInit()
  {
    //what for?
  }
	
  void inhibit()
  { _inhibited = true; }

  void doLoop()
  {
    // what's the point?
  }

  void doRelease(){}

  // overload
  virtual void start(bool wait=true)
  {
    // the thread is useless
  }

  void up()
  {
    if (_inhibited)
      return;
    
    _setPositions(_up);
  }

  void down()
  {
    if(_inhibited)
      return;

    _setPositions(_down);
  }

  void back()
  {
    if (_inhibited)
      return;
    
    _setPositions(_back);
  }

  void forth()
  {
    if (_inhibited)
      return;
    
    _setPositions(_forth);
  }
	
  void stopArm()
  {
    if (_inhibited)
      return;

    //    inhibit();
    
    arm->stopMotion();
  }

private:
#if 0
  void _setPositions(YVector &v)
  {
    for(int i = 1; i<=__nj; i++)
      printf("%lf\t", v(i));
    printf("\n");

    for(int k=0;k<4;k++)
      arm->setPosition(k, v(k+1));
  }
#endif

  void _setPositions(YVector &v)
  {
    arm->setPositions(v.Length(), v.data());
  }

  void _parseFile(char *path, char *file)
  {
    YARPConfigFile cfg(path, file);

    int n = __nj;
    int length;
    _down.Resize(__nj);
    _up.Resize(__nj);
    _back.Resize(__nj);
    _forth.Resize(__nj);
	
    cfg.get("[COMMANDS]", "Up=", _up.data(), __nj);
    cfg.get("[COMMANDS]", "Down=", _down.data(), __nj);
    cfg.get("[COMMANDS]", "Back=", _back.data(), __nj);
    cfg.get("[COMMANDS]", "Forth=", _forth.data(), __nj);
  }

  ArmDaemon *arm;
  
  YVector _up;
  YVector _down;
  YVector _back;
  YVector _forth;

  bool readyF;
  bool _inhibited;
};

int main()
{
  ACE_OS::printf("\nHello from YARP!\n\n");

  //set_yarp_debug(100,100);

  _inPort.Register("/armStepper/i:bot");

  // read positions from file
  char path[255];
  sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

  ArmDaemon arm;
  ArmController controller(&arm,3000);
  controller.load(path, "stepper.cfg");
	
  arm.Register("/armStepper/o:bot");
	
  printf("Listening...\n");

  bool exit=false;
  while(!exit)
    {
      _inPort.Read();

      YARPBottle & tmp = _inPort.Content();
		
      int iTmp;
      if(tmp.readInt(&iTmp))
	{
	  if (iTmp==7)
	    {	
	      if (tmp.readInt(&iTmp))
		{
		  printf("Message ID %d\n", iTmp);
		  if (iTmp==5)
		    controller.stopArm();
		  else if (iTmp==1)
		    controller.up();
		  else if (iTmp==2)
		    controller.down();
		  else if (iTmp=3)
		    controller.back();
		  else if (iTmp=4)
		    controller.forth();
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
	
  ACE_OS::printf("\ndone!\n");
	
  return 0;
}
