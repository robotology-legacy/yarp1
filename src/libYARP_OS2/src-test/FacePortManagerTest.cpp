#include <yarp/FacePortManager.h>
#include <yarp/Bottle.h>
#include <yarp/Time.h>
#include <yarp/Thread.h>
#include <yarp/NameClient.h>
#include <yarp/Companion.h>

#include "TestList.h"

using namespace yarp;

#define PORT1_NAME "/tmp/port/sock/9999"

class FacePortManagerTest : public UnitTest {
private:

  class Sender : public Thread {
  public:
    bool closed;

    FacePortManagerTest& owner;

    Sender(FacePortManagerTest& owner) : owner(owner) {
      closed = false;
    }

    virtual void run() {
      try {
	NameClient& nic = NameClient::getNameClient();
	Address address = nic.queryName("/write");
	Bottle bot;
	FacePortManager man(PORT1_NAME,address);
	man.setReader(bot);
	Thread t(&man);
	t.start();
	String s = "x";
	bool done = false;
	while (!done) {
	  for (int i=0; i<5; i++) {
	    Time::delay(0.1);
	    if (closed) {
	      done = true;
	      break;
	    }
	  }
	  if (done) break;
	  owner.report(0,"Sending a message...");
	  Bottle bot2;
	  bot2.addInt(0);
	  bot2.addString(s.c_str());
	  man.send(bot2);
	  s += "x";
	}
	//ACE_OS::printf("shutting down thread\n");
	t.close();
	//ACE_OS::printf("joining thread\n");
	t.join();
	//ACE_OS::printf("joined thread\n");
      } catch (IOException e) {
	owner.report(1,e.toString() + " <<< sender got exception");
      }
    }
    
    virtual void close() {
      closed = true;
    }
  };




  class Receiver : public Thread, public Readable {
  public:
    bool closed;

    FacePortManagerTest& owner;

    Receiver(FacePortManagerTest& owner) : owner(owner) {
      closed = false;
    }

    virtual void readBlock(BlockReader& reader) {
      Bottle bot;
      bot.readBlock(reader);
      ACE_OS::printf("receiver got something! %s\n",
		     bot.toString().c_str());
    }

    virtual void run() {
      try {
	NameClient& nic = NameClient::getNameClient();
	Address address = nic.queryName("/read");
	Bottle bot;
	FacePortManager man(PORT1_NAME,address);
	man.setReader(*this);
	Thread t(&man);
	t.start();
	bool done = false;
	while (true) {
	  Time::delay(0.1);
	  if (closed) {
	    done = true;
	    break;
	  }
	}
	ACE_OS::printf("level 2 Receiver closing a\n");
	t.close();
	ACE_OS::printf("level 2 Receiver closing b\n");
	t.join();
	ACE_OS::printf("level 2 Receiver closing c\n");
      } catch (IOException e) {
	owner.report(1,e.toString() + " <<< sender got exception");
      }
    }
    
    virtual void close() {
      closed = true;
    }
  };










public:
  virtual String getName() { return "FacePortManagerTest"; }

  void testBottle() {
    report(0,"simple bottle check...");
    NameClient& nic = NameClient::getNameClient();
    Address writeAddress = Address("localhost",9999,"tcp");
    Address readAddress = Address("localhost",9998,"tcp");
    Address fakeAddress = Address("localhost",9000,"tcp");
    nic.setFakeMode(true);
    nic.registerName("/write",writeAddress);
    nic.registerName("/read",readAddress);
    nic.registerName("/bozo",fakeAddress);
    checkEqual(nic.queryName("/write").isValid(),true,"name server sanity");
    try {
      Sender sender(*this);
      Receiver receiver(*this);
      sender.start();
      receiver.start();
      Time::delay(2);
      Companion::connect("/write","/read");
      Time::delay(2);
      ACE_OS::printf("Receiver closing\n");
      receiver.close();
      ACE_OS::printf("Receiver closed\n");
      Time::delay(5);
      sender.close();
      receiver.join();
      sender.join();
    } catch (IOException e) {
      report(1,e.toString() + " <<< testBottle got exception");
    }
    nic.setFakeMode(false);
    report(0,"made it through");
  }

  virtual void runTests() {
    report(0,"notice: these tcp tests rely on port 9998+9999 being free...");
    testBottle();
  }
};

static FacePortManagerTest theFacePortManagerTest;

UnitTest& getFacePortManagerTest() {
  return theFacePortManagerTest;
}
