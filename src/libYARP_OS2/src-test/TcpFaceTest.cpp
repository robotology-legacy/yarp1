#include <yarp/TcpFace.h>
#include <yarp/Thread.h>
#include <yarp/IOException.h>
#include <yarp/Time.h>

#include "TestList.h"

using namespace yarp;

class TcpFaceTest : public UnitTest {
private:

  class Listener : public Thread {
  private:
    TcpFaceTest& owner;
    TcpFace face;
  public:
    Listener(TcpFaceTest& owner) : owner(owner) {}

    ~Listener() {
      //ACE_OS::printf("Listener destroyed\n");
    }

    virtual void run() {
      face.open(Address("localhost",9999,"tcp"));
      //ACE_OS::printf("Listener waiting\n");
      try {
	InputProtocol *ip = face.read();
	//ACE_OS::printf("Listener got something\n");
	if (ip!=NULL) {
	  ip->close();
	  delete ip;
	}
      } catch (IOException e) {
	//ACE_OS::printf("Listener interrupted -- expected\n");
      }
    }

    virtual void close() {
      face.close();
    }
  };


public:
  virtual String getName() { return "TcpFaceTest"; }

  void testListenStartStop() {
    report(0,"testing starting/stopping tcp listening...");
    int tct = Thread::getCount();
    for (int i=0; i<1; i++) {
      Listener listen(*this);
      listen.start();
      //checkEqual(Thread::getCount(),tct+1,"thread count");
      Time::delay(0.33);
      //ACE_OS::printf("trying to stop listener\n");
      listen.close();
      //ACE_OS::printf("did close, joining...\n");
      //Time::delay(3);
      listen.join();
      //ACE_OS::printf("stopped\n");
    }
  }

  void testListen() {
    report(0,"testing tcp listening - relies on free ports...");
    /*
    try {
      TcpFace face;
      ACE_OS::printf("waiting\n");
      InputProtocol *ip = face.read();
      ACE_OS::printf("got something\n");
      //ip->close();
      ip->open("/cyarp/port");
      //BlockReader& br = ip->beginRead();
      //String s = br.expectLine();
      //ACE_OS::printf("got [%s]\n", s.c_str());
      //ip->endRead();
      ACE_OS::printf("done\n");  
      face.close();
    } catch (IOException e) {
      report(1,e.toString() + " <<< got exception");
    }
    */
  }

  void testConnect() {
    /*
    try {
      TcpFace ff;
      OutputProtocol *out = ff.write(Address("localhost",10002,"tcp"));
      Route route("/from","/to","text");
      out->open(route);
      BufferedBlockWriter bw;
      bw.appendLine("d");
      bw.appendLine("0 \"Hello World\"");
      out->write(bw);
      out->close();
      delete out;
    } catch (IOException e) {
      ACE_DEBUG((LM_ERROR,"tcp skipped, issue: %s", e.toString().c_str()));
    }
    */
  }

  virtual void runTests() {
    report(0,"Notice - tcp listening tests rely on port 9999 being free...");
    testListenStartStop();
    //testListen();
    //testConnect();
  }
};

static TcpFaceTest theTcpFaceTest;

UnitTest& getTcpFaceTest() {
  return theTcpFaceTest;
}

