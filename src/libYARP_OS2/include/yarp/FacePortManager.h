#ifndef _YARP2_FACEPORTMANAGER_
#define _YARP2_FACEPORTMANAGER_

#include <yarp/Logger.h>
#include <yarp/PortManager.h>
#include <yarp/Address.h>
#include <yarp/Carriers.h>
#include <yarp/Thread.h>
#include <yarp/InputConnection.h>

#include <ace/Vector_T.h>

namespace yarp {
  class FacePortManager;
}

/**
 * For the first time, we mix comms with threads.
 */
class yarp::FacePortManager : public PortManager, public Runnable {
public:
  FacePortManager(const Address& address) {
    face = Carriers::listen(address);
  }

  virtual ~FacePortManager() {
    close();
    closeInputs();
  }

  virtual void run();

  virtual void close() {
    if (face!=NULL) {
      face->close();
      delete face;
      face = NULL;
    }
  }

  virtual void readBlock(BlockReader& reader);

private:
  Face *face;

  class InputEntry : public Thread {
  public:
    InputEntry(InputProtocol *ip, FacePortManager *man) :
      connection(ip,man)
    {
      owner = man;
      start();
    }

    virtual void run();
    
    virtual void close();

  private:
    InputConnection connection;
    FacePortManager *owner;
  };

  ACE_Vector<InputEntry *> inputs;

  void closeInputs() {
    for (int i=0; i<inputs.size(); i++) {
      delete inputs[i];
    }
    inputs.clear();
  }

  void addInput(InputProtocol *ip) {
    InputEntry *entry = new InputEntry(ip,this);
    inputs.push_back(entry);
    ACE_OS::printf("There are %d input connections\n", inputs.size());
  }


  // public for now; use friends to hide
public:
  void removeInput(InputEntry *entry) {
    int k = -1;
    for (int i=0; i<inputs.size(); i++) {
      if (inputs[i]==entry) {
	k = i;
      }
    }
    if (k!=-1) {
      int last = inputs.size()-1;
      if (k!=last) {
	inputs[k] = inputs[last];
      }
      inputs.pop_back();
      delete entry;
    }
  }
};

#endif

