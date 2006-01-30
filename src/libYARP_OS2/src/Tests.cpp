
#include <yarp/Address.h>
#include <yarp/Carriers.h>
#include <yarp/Bytes.h>
#include <yarp/StringInputStream.h>
#include <yarp/StringOutputStream.h>
#include <yarp/FakeTwoWayStream.h>
#include <yarp/StreamBlockReader.h>
#include <yarp/BufferedBlockWriter.h>
#include <yarp/ShiftStream.h>
#include <yarp/Carrier.h>
#include <yarp/Protocol.h>
#include <yarp/TcpCarrier.h>
#include <yarp/TextCarrier.h>
#include <yarp/FakeFace.h>
#include <yarp/TcpFace.h>
#include <yarp/Companion.h>
#include <yarp/NameClient.h>
#include <yarp/Bottle.h>
#include <yarp/PortCommand.h>
#include <yarp/InputConnection.h>
#include <yarp/Time.h>
#include <yarp/Thread.h>
#include <yarp/Semaphore.h>
#include <yarp/FacePortManager.h>

#include <ace/OS_NS_stdio.h>
#include <ace/OS_NS_stdlib.h>

/*
 * Just the start of a test harness.
 * Should find some portable C++ equivalent of junit.
 */

using namespace yarp;

#define assertion(x,y) assertionImpl(x,y,#x,#y,__FILE__,__LINE__)

template <class T>
static void assertionImpl(const T& x, const T& y, 
			  const char *txt1, const char *txt2,
			  const char *fname, int fline) {
  if (x!=y) {
    ACE_OS::printf("Test FAILED! %s:%d %s != %s\n", fname, fline, txt1, txt2);
    ACE_OS::exit(1);
  } else {
    ACE_OS::printf("Test ok! %s:%d %s == %s\n", fname, fline, txt1, txt2);
  }
}


static void checkCarriers() {
  //Address address("localhost",10000,"tcp");
  Address address("localhost",10000,"fake");
  Face *face = Carriers::listen(address);
  InputProtocol *proto = face->read();
  delete proto;
  delete face;
}




static void checkFakeFace() {
  try {
    FakeFace ff;
    OutputProtocol *out = ff.write(Address("bozo",2,"text"));
    Route route("/from","/to","text");
    out->open(route);
    BufferedBlockWriter bw;
    bw.appendLine("Hello");
    out->write(bw);
    
    // begin HACK
    Protocol& proto = *((Protocol *)out);
    StringOutputStream& sos = (StringOutputStream&)(proto.os());
    ACE_OS::printf("Data is [%s]\n", sos.toString().c_str());
    // end HACK
    
    out->close();
    delete out;
  } catch (IOException e) {
    ACE_DEBUG((LM_ERROR,"skipped, issue: %s", e.toString().c_str()));
  }
}



static void checkNameClient() {
  const char *s="registration name /bozo ip 5.255.112.225 port 10002 type tcp";
  NameClient& nc = NameClient::getNameClient();
  Address a = nc.extractAddress(String(s));
  assertion(a.isValid(),true);
  ACE_OS::printf("address is %s\n", a.toString().c_str());
  a = nc.queryName("/read");
  ACE_OS::printf("address is %s\n", a.toString().c_str());
}

static void checkCompanion(int argc, char *argv[]) {
  Companion::main(argc,argv);
}

static void readTcpBottle() {
  TcpFace face;
  face.open(Address("localhost",8000,"tcp"));
  ACE_OS::printf("waiting\n");
  InputProtocol *ip = face.read();
  ACE_OS::printf("got something...\n");
  ip->open("/cyarp/port");
  BlockReader& br = ip->beginRead();
  try {
    while(true) {
      String s = br.expectLine();
      ACE_OS::printf("got [%s] should be just d\n", s.c_str());
      Bottle bot;
      bot.readBlock(br);
      ACE_OS::printf("hopefully got bottle [%s], %d elements\n", 
		     bot.toString().c_str(), bot.size());
    }
  } catch (IOException e) {
    ACE_OS::printf("issue: %s\n", e.toString().c_str());
  }
  ip->endRead();
  ACE_OS::printf("done\n");  
  face.close();
}


static void readTcpBottle2() {
  TcpFace face;
  face.open(Address("localhost",8000,"tcp"));
  ACE_OS::printf("waiting\n");
  InputProtocol *ip = face.read();
  ACE_OS::printf("got something...\n");
  PortManager pm;
  pm.setName("/cyarp/port");
  InputConnection con(ip,&pm);
  con.run();
  ACE_OS::printf("done\n");  
  face.close();
}




void checkFacePortManager() {
  Address address = Address("localhost",8000,"tcp");
  Bottle bot;
  FacePortManager man("/inny",address);
  man.setReader(bot);
  Thread t(&man);
  t.start();
  String s = "x";
  while (true) {
    Time::delay(3);
    ACE_OS::printf("Fire!\n");
    Bottle bot2;
    bot2.addInt(0);
    bot2.addString(s.c_str());
    man.send(bot2);
    s += "x";
  }
}

/**
 * This is a gateway for a test harness.
 * PENDING: This method should be moved into the yarp namespace.
 */
int yarp_test_main(int argc, char *argv[]) {
  if (argc<=1) {
    Logger::get().setVerbosity(5);
    ACE_OS::printf("yarp testing underway\n");
    readTcpBottle();
    readTcpBottle2();
    checkFacePortManager();
    checkCarriers();
    checkFakeFace();
    checkCompanion(argc,argv);
    checkNameClient();
    ACE_OS::printf("yarp testing done\n");
  } else {
    return Companion::main(argc,argv);
  }
  return 0;
}

