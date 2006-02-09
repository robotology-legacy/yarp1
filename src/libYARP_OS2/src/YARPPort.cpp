
///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #paulfitz, pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///


///
/// $Id: YARPPort.cpp,v 1.3 2006-02-09 15:33:34 eshuy Exp $
//
/// Based on: Id: YARPPort.cpp,v 2.0 2005/11/06 22:21:26 gmetta Exp
//
/// Now, this is a compatibility layer, building YARPPorts from the 
/// new PortCore infrastructure.
///

/**
 * \file YARPPort.cpp It contains the implementation of the YARPPort object
 * and some additional private helper classes.
 *
 */



#include <yarp/PortCore.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>
#include <yarp/Logger.h>
#include <yarp/Time.h>
#include <yarp/Semaphore.h>
using namespace yarp;


#define DEBUG_H_INC
#include <yarp/YARPPort.h>


#ifdef __WIN32__
// library initialization.
#pragma init_seg(lib)
#endif

using namespace std;

#ifndef DOXYGEN_SHOULD_SKIP_THIS

class BlockWriter_to_YARPPortWriter : public YARPPortWriter {
private:
  BlockWriter& writer;
public:
  BlockWriter_to_YARPPortWriter(BlockWriter& writer) : writer(writer) {}

  virtual ~BlockWriter_to_YARPPortWriter() {}

  virtual int Write(char *buffer, int length) {
    //ACE_OS::printf("Writing %d bytes\n", length);
    writer.appendBlock(Bytes(buffer,length));
    return 1;
  }
};

class BlockReader_to_YARPPortReader : public YARPPortReader {
private:
  BlockReader& reader;
public:
  BlockReader_to_YARPPortReader(BlockReader& reader) : reader(reader) {}

  virtual ~BlockReader_to_YARPPortReader() {}

  virtual int Read(char *buffer, int length) {
    //ACE_OS::printf("Reading %d bytes\n", length);
    reader.expectBlock(Bytes(buffer,length));
    return 1;
  }
};

class WritableContent : public Writable {
private:
  YARPPortContent& content;
public:
  WritableContent(YARPPortContent& content) : content(content) {}

  virtual ~WritableContent() {}

  virtual void writeBlock(BlockWriter& writer) {
    BlockWriter_to_YARPPortWriter delegate(writer);
    content.Write(delegate);
  }
};


class ReadableContent : public Readable {
private:
  YARPPortContent& content;
public:
  ReadableContent(YARPPortContent& content) : content(content) {}

  virtual ~ReadableContent() {}

  virtual void readBlock(BlockReader& reader) {
    BlockReader_to_YARPPortReader delegate(reader);
    content.Read(delegate);
  }
};


/**
 * PortData is a simple convenient container for the generic Port class.
 */
class PortData : public Readable
{
public:
  PortCore core;
  Semaphore incoming;

  PortData() : incoming(0) {
    ypc = NULL;
  }

  virtual ~PortData() {
    if (ypc!=NULL) {
      delete ypc;
      ypc = NULL;
    }
  }

  /** The owner, if input port. */
  YARPInputPort *in_owner;
  
  /** The owner, if output port. */
  YARPOutputPort *out_owner;

  YARPPortContent *ypc;
  
  /** The OnRead() callback function. */
  virtual void OnRead() { if (in_owner!=NULL) in_owner->OnRead(); }
  
  /** The OnWrite() callback function. */
  virtual void OnSend() { if (out_owner!=NULL) out_owner->OnWrite(); }

  void End() {
    core.close();
  }

  int SetName (const char *name, const char *net_name) {
    NameClient& nic = NameClient::getNameClient();
    Address address = nic.registerName(name);
    int result = core.listen(address);
    core.setReadHandler(*this);
    core.start();
    return result;
  }

  int CountClients() {
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    ACE_OS::exit(1);
    return 0;
  }

  int IsSending() {
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    ACE_OS::exit(1);
    return 0;
  }

  void FinishSend() {
    YARP_DEBUG(Logger::get(),"FinishSend does not need to do anything at the time of writing");
  }

  int Say(const char *str) {
    return Companion::connect(core.getName().c_str(),str);
  }

  void Deactivate() {
    core.close();
  }

  void Send() {
    YARPPortContent *content = ypc;
    YARP_ASSERT(content!=NULL);
    WritableContent wc(*content);
    //ACE_OS::printf("writing!\n");
    core.send(wc);
  }

  int Read(bool wait) {
    //ACE_OS::printf("reading!\n");
    if (wait) {
      incoming.wait();
      //ACE_OS::printf("got something!\n");
      return 1;
    } else {
      int result = incoming.check();
      if (result) {
	while (incoming.check()) {
	  // blow away back log
	}
      }
      //ACE_OS::printf("got %d!\n", result);
      return result;
    }
    return 1;
  }

  virtual void readBlock(BlockReader& reader) {
    //ACE_OS::printf("got some data!\n");
    if (ypc!=NULL) {
      //ACE_OS::printf("there is some content too so can read\n");
      ReadableContent rc(*ypc);
      rc.readBlock(reader);
      incoming.post();
    }
  }
};

PortData& CastPortData(void *system_resource)
{
	ACE_ASSERT(system_resource!=NULL);
	return *((PortData *)system_resource);
}

#define PD CastPortData(system_resource)

#endif /* DOXYGEN_SHOULD_SKIP_THIS */


YARPPort::YARPPort()
{
	system_resource = new PortData;
	ACE_ASSERT(system_resource!=NULL);
	PD.in_owner = NULL;
	PD.out_owner = NULL;
}


YARPPort::~YARPPort()
{
	if (system_resource != NULL)
		delete ((PortData*)system_resource);
}


void YARPPort::End()
{
  PD.End();
}

int YARPPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
  Content();
  return 0==(PD.SetName (name, net_name));
}

int YARPPort::Unregister(void)
{
  PD.End ();
  return YARP_OK;
}

int YARPPort::IsReceiving()
{
  return PD.CountClients();
}

int YARPPort::IsSending()
{
  return PD.IsSending();
}

void YARPPort::FinishSend()
{
  PD.FinishSend();
}

int YARPPort::Connect(const char *name)
{
  int result = PD.Say(name);
  return (result==0);
}


int YARPPort::Connect(const char *src_name, const char *dest_name)
{
  int result = Companion::connect(src_name,dest_name);
  return (result==0);
}


YARPPortContent& YARPPort::Content()
{
  if (PD.ypc==NULL) {
    PD.ypc = CreateContent();
  }
  content = PD.ypc;
  return *(PD.ypc);
}


void YARPPort::Deactivate()
{
	PD.Deactivate();
}


void YARPPort::DeactivateAll()
{
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    ACE_OS::exit(1);
}


void YARPPort::SetRequireAck(int require_ack) 
{
    ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
    //  PD.SetRequireAck(require_ack);
}


int YARPPort::GetRequireAck()
{
  ACE_OS::printf("%s:%d -- not implemented\n",__FILE__,__LINE__);
  return 1;
  //return PD.GetRequireAck();
}



YARPInputPort::YARPInputPort(int n_service_type, int n_protocol_type)
{
  //PD.service_type = n_service_type;
  //PD.protocol_type = n_protocol_type;
  PD.in_owner = this;
}


YARPInputPort::~YARPInputPort()
{
}


int YARPInputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
  /*
	int service_type = PD.service_type;
	PD.TakeReceiverIncoming(new YARPSendable(CreateContent()));
	if (service_type == DOUBLE_BUFFERS || service_type == TRIPLE_BUFFERS)
	{
		PD.TakeReceiverLatest(new YARPSendable(CreateContent()));
	}
	if (service_type == TRIPLE_BUFFERS)
	{
		PD.TakeReceiverAccess(new YARPSendable(CreateContent()));
	}
  */
	return YARPPort::Register(name, net_name);
}


bool YARPInputPort::Read(bool wait)
{
  Content();
  return PD.Read(wait);
}


YARPOutputPort::YARPOutputPort(int n_service_type, int n_protocol_type)
{
  PD.out_owner = this;
}


YARPOutputPort::~YARPOutputPort()
{
}


int YARPOutputPort::Register(const char *name, const char *net_name /* = YARP_DEFAULT_NET */)
{
  return YARPPort::Register(name, net_name);
}


YARPPortContent& YARPOutputPort::Content()
{
  return YARPPort::Content();
}



void YARPOutputPort::Write(bool wait)
{
  PD.Send();
}


void YARPOutputPort::SetAllowShmem(int flag)
{
  ACE_OS::printf("%s:%d -- shared memory not implemented\n",__FILE__,__LINE__);
  //PD.SetAllowShmem(flag);
}


int YARPOutputPort::GetAllowShmem(void)
{
  ACE_OS::printf("%s:%d -- shared memory not implemented\n",__FILE__,__LINE__);
  //return PD.GetAllowShmem();
  return 0;
}
