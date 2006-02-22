#ifndef _YARP2_PORTCORE_
#define _YARP2_PORTCORE_

#include <yarp/ThreadImpl.h>
#include <yarp/SemaphoreImpl.h>
#include <yarp/Carriers.h>
#include <yarp/Address.h>
#include <yarp/PortManager.h>
#include <yarp/Readable.h>
#include <yarp/Writable.h>


namespace yarp {
  class PortCore;
  class PortCoreUnit;
}

/**
 * This is the heart of a yarp port.  It is the thread manager.
 * All other port components are insulated from threading
 * It maintains a collection of incoming and
 * outgoing connections.  Data coming from incoming connections is
 * directed to the handler set with setReadHandler().  Calls to send()
 * result in data begin sent to all the outgoing connections.
 */
class yarp::PortCore : public ThreadImpl, public PortManager, public Readable {
public:

  PortCore() {
    // dormant phase
    listening = false;
    running = false;
    starting = false;
    closing = false;
    finished = false;
    autoHandshake = true;
    events = 0;
    face = NULL;
    reader = NULL;
  }

  virtual ~PortCore();

  // configure core
  bool listen(const Address& address);

  void setReadHandler(Readable& reader);

  void setAutoHandshake(bool autoHandshake) {
    this->autoHandshake = autoHandshake;
  }

  virtual bool read(ConnectionReader& reader) {
    // does nothing by default
    return true;
  }

  // start up core
  virtual bool start();

  // use port as output
  void send(Writable& writer);

  // shut down and deconfigure core
  virtual void close();

  // main manager thread
  virtual void run();

  // useful for stress-testing
  int getEventCount();

  const Address& getAddress() const {
    return address;
  }

public:

  // PortManager interface, exposed to inputs

  virtual void addOutput(const String& dest, void *id, OutputStream *os);
  virtual void removeOutput(const String& dest, void *id, OutputStream *os);
  virtual void removeInput(const String& dest, void *id, OutputStream *os);
  virtual void describe(void *id, OutputStream *os);
  virtual void readBlock(ConnectionReader& reader, void *id, OutputStream *os);

private:

  // internal maintenance of sub units

  ACE_Vector<PortCoreUnit *> units;

  // only called in "finished" phase
  void closeUnits();

  // called anytime, garbage collects terminated units
  void cleanUnits();

  // only called by the manager
  void reapUnits();

  // only called in "running" phase
  void addInput(InputProtocol *ip);

  void addOutput(OutputProtocol *op);

  bool removeUnit(const Route& route);

private:

  // main internal PortCore state and operations
  SemaphoreImpl stateMutex;
  Face *face;
  String name;
  Address address;
  Readable *reader;
  bool listening, running, starting, closing, finished, autoHandshake;
  int events;

  void closeMain();
};

#endif
