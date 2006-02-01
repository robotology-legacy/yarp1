

#include <yarp/Time.h>
#include <yarp/PortCoreOutputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;

bool PortCoreOutputUnit::start() {

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


void PortCoreOutputUnit::run() {
  running = true;
  phase.post();

  runSimulation();

  // it would be nice to get my entry removed from the port immediately,
  // but it would be a bit dodgy to delete this object and join this
  // thread within and from themselves
}



void PortCoreOutputUnit::runSimulation() {
  // simulation
  running = true;
  while (true) {
    ACE_OS::printf("tick\n");
    Time::delay(0.3);
    if (closing||isDoomed()) {
      break;
    }
  }

  ACE_OS::printf("stopping\n");

  running = false;
  finished = true;
}


void PortCoreOutputUnit::closeMain() {

  if (running) {
    // give a kick (unfortunately unavoidable)
    /*
    if (op!=NULL) {
      op->interrupt();
    }
    */
    closing = true;
    join();
  }

  if (op!=NULL) {
    try {
      op->close();
    } catch (IOException e) { /*ok*/ }
    try {
      delete op;
    } catch (IOException e) { /*ok*/ }
    op = NULL;
  }
  running = false;
  closing = false;
  finished = false;
}


Route PortCoreOutputUnit::getRoute() {
  if (op!=NULL) {
    return op->getRoute();
  }
  return PortCoreUnit::getRoute();
}


