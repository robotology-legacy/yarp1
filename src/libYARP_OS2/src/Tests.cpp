
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


static void checkString() {
  // unfortunately need String to be able to include \0 parts
  String s;
  s += 'h';
  s += '\0';
  s += 'd';
  assertion((int)s.length(),3);
  assertion(s[1],'\0');
}


static void checkAddress() {
  Address address("localhost",10000,"tcp");
  String txt = address.toString();
  assertion(txt,String("tcp:/localhost:10000"));
}

static void checkCarriers() {
  //Address address("localhost",10000,"tcp");
  Address address("localhost",10000,"fake");
  Face *face = Carriers::listen(address);
  InputProtocol *proto = face->read();
  delete face;
}

static void checkInputStreams() {
  StringInputStream sis;
  sis.add("Hello my friend");
  char buf[256];
  sis.check();
  Bytes b(buf,sizeof(buf));
  int len = sis.read(b,0,5);
  assertion(len,5);
  buf[len] = '\0';
  assertion(String("Hello"),String(buf));
  char ch = sis.read();
  assertion(ch,' ');
  len = sis.read(b,0,2);
  assertion(len,2);
  buf[len] = '\0';
  assertion(String("my"),String(buf));
  /*
  try {
    len = sis.read(b,0,100);
    assertion(String("should have thrown an exception"),String(""));
  } catch (IOException e) {
    ACE_OS::printf("Got an exception, good: %s\n", e.toString().c_str());
  }
  */
}


static void checkOutputStreams() {
  StringOutputStream sos;
  char txt[] = "Hello my friend";
  Bytes b(txt,ACE_OS::strlen(txt));
  sos.write(b);
  assertion(String(txt),sos.toString());
  StringOutputStream sos2;
  sos2.write('y');
  sos2.write('o');
  assertion(String("yo"),sos2.toString());
}


static void checkTwoWayStreams() {
  int len;
  FakeTwoWayStream tw;
  char txt[] = "Hello my friend";
  Bytes b(txt,ACE_OS::strlen(txt));
  tw.getOutputStream().write(b);
  char buf[256];
  len = tw.getInputStream().read(Bytes(buf,sizeof(buf)));
  assertion(len,(int)(ACE_OS::strlen(txt)));
  buf[len] = '\0';
  assertion(String(txt),String(buf));  
}


static void checkBlocks() {
  StringInputStream sis;
  sis.add("Hello\ngood evening and welcome");
  StreamBlockReader sbr;
  sbr.reset(sis,10,true);
  String line = sbr.expectLine();
  assertion(line,String("Hello"));

  StringOutputStream sos;

  BufferedBlockWriter bbr;
  bbr.reset(true);
  bbr.appendLine("Hello");
  bbr.appendLine("Greetings");
  bbr.write(sos);
  //ACE_OS::printf("text is %s\n", sos.toString().c_str());
  assertion(sos.toString(),String("Hello\nGreetings\n"));
}

/**
 * This is a gateway for a test harness.
 * PENDING: This method should be moved into the yarp namespace.
 */
int yarp_test_main(int argc, char *argv[]) {
  ACE_OS::printf("yarp testing underway\n");
  checkString();
  checkAddress();
  checkInputStreams();
  checkOutputStreams();
  checkTwoWayStreams();
  checkBlocks();
  //checkCarriers();
  ACE_OS::printf("yarp testing done\n");
  return 0;
}

