
#include <yarp/Time.h>
#include <yarp/PortCoreInputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;

bool PortCoreInputUnit::start() {

  phase.wait();

  bool result = PortCoreUnit::start();
  if (result) {
    phase.wait();
    phase.post();
  } else {
    phase.post();
  }

  return result;
}


void PortCoreInputUnit::run() {
  running = true;
  phase.post();

  try {
    bool done = false;

    YARP_ASSERT(ip!=NULL);

    PortCommand cmd;
  
    ip->open(getName().c_str());
    if (closing) {
      done = true;
    }

    void *id = (void *)this;

    while (!done) {
      BlockReader& br = ip->beginRead();
      cmd.readBlock(br);
      if (closing||isDoomed()) {
	done = true;
	break;
      }
      char key = cmd.getKey();
      //ACE_OS::printf("Port command is [%c:%d/%s]\n",
      //	     (key>=32)?key:'?', key, cmd.getText().c_str());

      PortManager& man = getOwner();
      OutputStream *os = NULL;
      if (br.isTextMode()) {
	os = &(ip->getOutputStream());
      }

      switch (key) {
      case '/':
	//ACE_OS::printf("asking to add output\n");
	man.addOutput(cmd.getText(),id,os);
	break;
      case '!':
	man.removeOutput(cmd.getText().substring(1,-1),id,os);
	break;
      case '~':
	man.removeInput(cmd.getText().substring(1,-1),id,os);
	break;
      case '*':
	man.describe(id,os);
	break;
      case 'd':
	man.readBlock(br,id,os);
	break;
      case 'q':
	done = true;
	break;
      }
      ip->endRead();
      if (closing||isDoomed()) {
	done = true;
	break;
      }
    }
  } catch (IOException e) {
    /* ok, ports die - it is their nature */
    ACE_OS::printf("PortCoreInputUnit got exception: %s\n", e.toString().c_str());
    ACE_DEBUG((LM_DEBUG,"PortCoreInputUnit got exception: %s\n",
	       e.toString().c_str()));
  }

  ip->close();

  running = false;
  finished = true;

  // it would be nice to get my entry removed from the port immediately,
  // but it would be a bit dodgy to delete this object and join this
  // thread within and from themselves
}



void PortCoreInputUnit::runSimulation() {
  // simulation
  running = true;
  while (true) {
    ACE_OS::printf("tick\n");
    Time::delay(0.3);
    if (closing) {
      break;
    }
  }

  ACE_OS::printf("stopping\n");

  running = false;
  finished = true;
}


void PortCoreInputUnit::closeMain() {

  if (running) {
    // give a kick (unfortunately unavoidable)
    if (ip!=NULL) {
      ip->interrupt();
    }
    closing = true;
    join();
  }

  if (ip!=NULL) {
    try {
      ip->close();
    } catch (IOException e) { /*ok*/ }
    try {
      delete ip;
    } catch (IOException e) { /*ok*/ }
    ip = NULL;
  }
  running = false;
  closing = false;
  finished = false;
  setDoomed(false);
}


Route PortCoreInputUnit::getRoute() {
  if (ip!=NULL) {
    return ip->getRoute();
  }
  return PortCoreUnit::getRoute();
}


