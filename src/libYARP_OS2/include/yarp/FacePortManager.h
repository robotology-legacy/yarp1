#ifndef _YARP2_FACEPORTMANAGER_
#define _YARP2_FACEPORTMANAGER_

#include <yarp/Logger.h>
#include <yarp/PortManager.h>
#include <yarp/Address.h>
#include <yarp/Carriers.h>
#include <yarp/Thread.h>
#include <yarp/InputConnection.h>
#include <yarp/Portable.h>

#include <ace/Vector_T.h>

namespace yarp {
  class FacePortManager;
}

/**
 * For the first time, we mix comms with threads.
 */
class yarp::FacePortManager : public PortManager, public Runnable {
public:
  FacePortManager(const String& name, const Address& address) {
    this->name = name;
    this->address = address;
    reader = NULL;
    face = Carriers::listen(address);
  }

  void setReader(Readable& reader) {
    this->reader = &reader;
  }

  virtual ~FacePortManager() {
    close();
  }

  virtual String getName() {
    return name;
  }

  virtual void run();

  virtual void close() {
    if (face!=NULL) {
      face->close();
      delete face;
      face = NULL;
    }
    closeInputs();
    closeOutputs();
  }

  virtual void readBlock(BlockReader& reader);

  virtual void send(Writable& writer);

  virtual void describe();

  virtual void removeInput(const String& src);

  virtual void addOutput(const String& dest);

  virtual void removeOutput(const String& dest);


private:
  Face *face;
  Readable *reader;
  String name;
  Address address;

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

    InputConnection& getConnection() {
      return connection;
    }

  private:
    InputConnection connection;
    FacePortManager *owner;
  };


  // dump, unthreaded way first
  class OutputEntry {
  public:
    OutputEntry(const String& name, OutputProtocol *op, FacePortManager *man) {
      owner = man;
      this->op = op;
      YARP_ASSERT(op!=NULL);
      YARP_ASSERT(man!=NULL);
      op->open(Route(man->getName(),name,"text"));
    }

    virtual ~OutputEntry() {
      close();
    }

    //virtual void run();

    virtual void write(SizedWriter& writer);

    virtual void askForClose() {
      close();
      owner->removeOutput(this);
    }
    
    virtual void close() {
      if (op!=NULL) {
	op->close();
	delete op;
	op = NULL;
      }
    }

    const Route& getRoute() {
      YARP_ASSERT(op!=NULL);
      return op->getRoute();
    }

  private:
    OutputProtocol *op;
    FacePortManager *owner;
  };


  ACE_Vector<InputEntry *> inputs;
  ACE_Vector<OutputEntry *> outputs;

  void closeInputs() {
    // TODO - should join with all threads
    for (int i=0; i<inputs.size(); i++) {
      delete inputs[i];
    }
    inputs.clear();
  }

  void closeOutputs() {
    for (int i=0; i<outputs.size(); i++) {
      delete outputs[i];
    }
    outputs.clear();
  }

  void addInput(InputProtocol *ip) {
    InputEntry *entry = new InputEntry(ip,this);
    inputs.push_back(entry);
    ACE_OS::printf("There are %d input connections\n", inputs.size());
  }

  void addOutput(const String& name, OutputProtocol *op) {
    OutputEntry *entry = new OutputEntry(name,op,this);
    outputs.push_back(entry);
    ACE_OS::printf("There are %d output connections\n", outputs.size());
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

  void removeOutput(OutputEntry *entry) {
    int k = -1;
    for (int i=0; i<outputs.size(); i++) {
      if (outputs[i]==entry) {
	k = i;
      }
    }
    if (k!=-1) {
      int last = outputs.size()-1;
      if (k!=last) {
	outputs[k] = outputs[last];
      }
      outputs.pop_back();
      delete entry;
    }
  }
};

#endif

