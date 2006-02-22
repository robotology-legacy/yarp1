
#include <yarp/os/Time.h>
#include <yarp/PortCoreInputUnit.h>
#include <yarp/PortCommand.h>
#include <yarp/Logger.h>
#include <yarp/Name.h>


#define YMSG(x) ACE_OS::printf x;
#define YTRACE(x) YMSG(("at %s\n",x))


using namespace yarp;

bool PortCoreInputUnit::start() {

  if (ip!=NULL) {
    Route route = ip->getRoute();
    YARP_DEBUG(Logger::get(),String("starting output for ") + 
	       route.toString());
  }

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

  Route route;

  try {
    bool done = false;

    YARP_ASSERT(ip!=NULL);

    PortCommand cmd;
  
    if (autoHandshake) {
      ip->open(getName().c_str());
      route = ip->getRoute();
      if (Name(route.getFromName()).isRooted()) {
	YARP_INFO(Logger::get(),String("Receiving input from ") + 
		  route.getFromName() + " to " + route.getToName() + 
		  " using " +
		  route.getCarrierName());
      }
    } else {
      ip->open(""); // anonymous connection
      route = ip->getRoute();
    }

    if (closing) {
	done = true;
    }

    void *id = (void *)this;

    while (!done) {
      try {
	ConnectionReader& br = ip->beginRead();

	if (autoHandshake) {
	  cmd.readBlock(br);
	} else {
	  cmd = PortCommand('d',"");
	}

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
	  YARP_DEBUG(Logger::get(),String("asking to add output to ")+
		     cmd.getText());
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
	  {
	    try {
	      man.readBlock(br,id,os);
	    } catch (IOException e) {
	      YARP_DEBUG(Logger::get(),e.toString() + " <<< user level PortCoreInputUnit exception, passing on");
	      done = true;
	      throw e;
	    }
	  }
	  break;
	case 'q':
	  done = true;
	  break;
	}
	ip->endRead();
      } catch (IOException e) {
	YARP_DEBUG(Logger::get(),e.toString() + " <<< initial PortCoreInputUnit exception");
	if (!ip->checkStreams()) {
	  // pass it on
	  YARP_DEBUG(Logger::get(), "passing on exception");
	  throw e;
	} else {
	  // clear out any garbage
	  ip->resetStreams();
	}
      }
      if (closing||isDoomed()||(!ip->checkStreams())) {
	done = true;
	break;
      }
    }
  } catch (IOException e) {
    /* ok, ports die - it is their nature */
    YARP_DEBUG(Logger::get(),e.toString() + " <<< PortCoreInputUnit exception");
    ACE_DEBUG((LM_DEBUG,"PortCoreInputUnit got exception: %s\n",
	       e.toString().c_str()));
  }

  
  YARP_DEBUG(Logger::get(),"PortCoreInputUnit closing ip");
  ip->close();
  YARP_DEBUG(Logger::get(),"PortCoreInputUnit closed ip");

  if (autoHandshake) {
    if (Name(route.getFromName()).isRooted()) {
      YARP_INFO(Logger::get(),String("Removing input from ") + 
		route.getFromName() + " to " + route.getToName());
    }
  } else {
    YARP_DEBUG(Logger::get(),"PortCoreInputUnit shutting down");
  }

  running = false;
  finished = true;

  // it would be nice to get my entry removed from the port immediately,
  // but it would be a bit dodgy to delete this object and join this
  // thread within and from themselves
}



void PortCoreInputUnit::runSimulation() {
  /*
  // simulation
  running = true;
  while (true) {
    ACE_OS::printf("tick\n");
    Time::delay(0.3);
    if (closing) {
      break;
    }
  }
  */

  ACE_OS::printf("stopping\n");

  running = false;
  finished = true;
}


void PortCoreInputUnit::closeMain() {
  YARP_DEBUG(Logger::get(),"PortCoreInputUnit closing");

  if (running) {
    // give a kick (unfortunately unavoidable)
    if (ip!=NULL) {
      YARP_DEBUG(Logger::get(),"PortCoreInputUnit interrupting");
      ip->interrupt();
      YARP_DEBUG(Logger::get(),"PortCoreInputUnit interrupted");
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
  //finished = false;
  //setDoomed(false);
}


Route PortCoreInputUnit::getRoute() {
  if (ip!=NULL) {
    return ip->getRoute();
  }
  return PortCoreUnit::getRoute();
}


