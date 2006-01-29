#ifndef _YARP2_PORTMANAGER_
#define _YARP2_PORTMANAGER_

#include <yarp/String.h>
#include <yarp/OutputStream.h>
#include <yarp/BlockReader.h>
#include <yarp/Logger.h>

namespace yarp {
  class PortManager;
}

class yarp::PortManager {
public:
  PortManager() {
    os = NULL;
    name = "null";
  }

  virtual ~PortManager() {
  }

  void setName(const String& name) {
    this->name = name;
  }

  virtual void addOutput(const String& dest) {
    ACE_OS::printf("ADDOUTPUT\n");
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to addOutput [%s]\n",
	       getName().c_str(),
	       dest.c_str()));
  }

  virtual void removeInput(const String& src) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeInput [%s]\n",
	       getName().c_str(),
	       src.c_str()));
  }

  virtual void removeOutput(const String& dest) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to removeOutput [%s]\n",
	       getName().c_str(),
	       dest.c_str()));
  }
  
  virtual void describe() {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to describe itself\n",
	       getName().c_str()));
  }

  virtual void readBlock(BlockReader& reader) {
    ACE_DEBUG((LM_ERROR,"PortManager for [%s] asked to deal with data\n",
	       getName().c_str()));
  }

  // may wish to make comments
  virtual void replyTo(OutputStream *os) {
    this->os = os;
  }

  virtual String getName() {
    return String(name);
  }

protected:
  bool hasOutput() {
    return os!=NULL;
  }

  OutputStream& getOutputStream() {
    YARP_ASSERT(os!=NULL);
    return *os;
  }

private:
  OutputStream *os;
  String name;
};

#endif

