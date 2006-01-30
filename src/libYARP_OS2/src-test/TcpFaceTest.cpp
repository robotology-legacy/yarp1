#include <yarp/TcpFace.h>
#include <yarp/Thread.h>
#include <yarp/IOException.h>
#include <yarp/Time.h>
#include <yarp/BufferedBlockWriter.h>

#include "TestList.h"

using namespace yarp;

class TcpFaceTest : public UnitTest {
private:

  class Listener : public Thread {
  private:
    TcpFaceTest& owner;
    TcpFace face;
    bool closed;
    String mode;
  public:
    Listener(TcpFaceTest& owner, const char *mode = "auto-abort") : owner(owner) { 
      closed = false; 
      this->mode = mode;
    }

    ~Listener() {
      //ACE_OS::printf("Listener destroyed\n");
    }

    virtual void run() {
      face.open(Address("localhost",9999,"tcp"));
      bool done = false;
      while (!done) {
	try {
	  //ACE_OS::printf("Listener waiting\n");
	  InputProtocol *ip = face.read();
	  //ACE_OS::printf("Listener got something, mode is %s\n",
	  //	 mode.c_str());
	  if (ip!=NULL) {
	    owner.counter++;
	    if (mode == "auto-abort") {
	      ip->close();
	      delete ip;
	      ip = NULL;
	    } else {
	      //ACE_OS::printf("Listener expecting data\n");
	      ip->open("noname");
	      BlockReader& br = ip->beginRead();
	      String s = br.expectLine();
	      owner.checkEqual(s.c_str(),"d","command message part");
	      s = br.expectLine();
	      owner.checkEqual(s.c_str(),"0 \"Hello World\"",
			       "payload message part");
	      ip->endRead();
	      ip->close();
	      delete ip;
	    }
	  }
	  //ACE_OS::printf("Listener done\n");
	} catch (IOException e) {
	  //ACE_OS::printf("Listener interrupted -- expected -- %s\n",
	  //	 e.toString().c_str());
	  done = true;
	}
      }
    }

    virtual void close() {
      face.close();
    }
  };


public:
  int counter;

  TcpFaceTest() {
    counter = 0;
  }

  virtual String getName() { return "TcpFaceTest"; }

  void testListenStartStop() {
    report(0,"testing starting/stopping tcp listening...");
    int tct = Thread::getCount();
    for (int i=0; i<3; i++) {
      Listener listen(*this);
      listen.start();
      checkEqual(Thread::getCount(),tct+1,"thread count");
      Time::delay(0.33);
      listen.close();
      listen.join();
    }
  }

  void testConnect() {
    report(0,"testing connecting...");
    try {
      int ct = counter;
      TcpFace face;
      Listener listen(*this);
      listen.start();
      Time::delay(0.33);

      int repeats = 3;
      for (int i=0; i<repeats; i++) {
	OutputProtocol *out = face.write(Address("localhost",9999,"tcp"));
	Time::delay(0.33);
	out->close();
	delete out;
	out = NULL;
      }

      listen.close();
      listen.join();
      checkEqual(counter,ct+repeats,"connection count check");

    } catch (IOException e) {
      report(1,e.toString() + " <<< got exception");
    }
  }


  void testConnectData() {
    report(0,"testing connecting and sending data...");
    try {
      TcpFace face;
      Listener listen(*this,"data");
      listen.start();
      Time::delay(0.33);

      int repeats = 3;
      for (int i=0; i<repeats; i++) {
	//ACE_OS::printf("trying to write\n");
	OutputProtocol *out = face.write(Address("localhost",9999,"tcp"));

	Route route("/from","/to","text");
	out->open(route);
	BufferedBlockWriter bw;
	bw.appendLine("d");
	bw.appendLine("0 \"Hello World\"");
	out->write(bw);
	
	Time::delay(0.33);
	out->close();
	delete out;
	out = NULL;
      }

      listen.close();
      listen.join();

    } catch (IOException e) {
      report(1,e.toString() + " <<< got exception");
    }
  }


  virtual void runTests() {
    report(0,"Notice - tcp listening tests rely on port 9999 being free...");
    testListenStartStop();
    testConnect();
    testConnectData();
  }
};

static TcpFaceTest theTcpFaceTest;

UnitTest& getTcpFaceTest() {
  return theTcpFaceTest;
}

