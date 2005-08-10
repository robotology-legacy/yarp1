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
    _mutex.Wait();
    Content().writeInt(5);	// arm daemon code
    Content().writeInt(0);	// activate PID command
    Write(0);	// send
    _mutex.Post();
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
    Content().writeInt(1);
    Content().writeDoubleVector(v, 6);
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
    readyF = false;
    _inhibited = true;
  }

  int load(char *path, char *name)
  {
    _parseFile(path, name, posList);
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

    for(int i = 1; i<=__nj; i++)
      printf("%lf\t", v(i));
    printf("\n");

    arm->setPositions(__nj, v.data());
		
  }

  void _parseFile(char *path, char *file, myList &list)
  {
    YARPConfigFile cfg(path, file);
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

  ArmDaemon *arm;
  
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

  //set_yarp_debug(100,100);

  _inPort.Register("/sweepNet/i:bot");

  // read positions from file
  char path[255];
  sprintf(path,"%s/%s", GetYarpRoot(), ConfigFilePath);

  ArmDaemon arm;
  ArmController controller(&arm,3000);
  controller.load(path, "sweep.cfg");
	
  arm.Register("/sweepNet/o:bot");
  // ACE_OS::printf("Please connect me to the arm daemon and hit a key");
  //  char c;
  //  cin >> c;
	
  //arm.activatePID();
  YARPTime::DelayInSeconds(2.0);

  printf("Listening...\n");

  bool inhibit = false;

  bool exit=false;
  while(!exit)
    {
      _inPort.Read();

      YARPBottle & tmp = _inPort.Content();
		
      int iTmp;
      if(tmp.readInt(&iTmp))
	{
	  if (iTmp==3)
	    {	
	      if (tmp.readInt(&iTmp))
		{
		  printf("Message ID %d\n", iTmp);
#ifndef NO_ARM
		  if ( (iTmp==2)&&(!inhibit))
		    {
		      // stop arm
		      controller.stopArm();
		    }
		  else if ( (iTmp==1)&&(!inhibit))
		    {
		      // start arm
		      controller.ready();
		      controller.start();	// wait
		    }
		  else if ( (iTmp==3)&&(!inhibit) )
		    {
		      controller.stopArm();
		      controller.ready();
		    }
		  else if (iTmp==4)
		    {
		      controller.park();
		    }
		  else if (iTmp==5)
		    {
		      if (inhibit)
			inhibit=false;
		      else
			inhibit=true;
		    }
		      
#endif
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
	
  ACE_OS::printf("\ndone!\n");
	
  return 0;
}
