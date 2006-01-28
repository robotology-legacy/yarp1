
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
  /*
  char buf[256];
  len = tw.getInputStream().read(Bytes(buf,sizeof(buf)));
  assertion(len,(int)(ACE_OS::strlen(txt)));
  buf[len] = '\0';
  assertion(String(txt),String(buf));  
  */
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

static void checkTcpFace() {
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
}


static String simplify(String x) {
  String result("");
  for (int i=0; i<x.length(); i++) {
    char ch = x[i];
    if (ch == '\n') {
      result += "\\n";
    } else {
      result += ch;
    }
  }
  return result;
}

static void show(FakeTwoWayStream *fake1, FakeTwoWayStream *fake2) {
  ACE_OS::printf("fake1  //  in: [%s]  //  out: [%s]\n",
		 simplify(fake1->getInputText()).c_str(),
		 simplify(fake1->getOutputText()).c_str());
  ACE_OS::printf("fake2  //  in: [%s]  //  out: [%s]\n\n",
		 simplify(fake2->getInputText()).c_str(),
		 simplify(fake2->getOutputText()).c_str());
}

static void checkProtocol() {
  try {
    FakeTwoWayStream *fake1 = new FakeTwoWayStream();
    FakeTwoWayStream *fake2 = new FakeTwoWayStream();
    fake1->setTarget(fake2->getStringInputStream());
    fake2->setTarget(fake1->getStringInputStream());
    Protocol p1(fake1);
    Protocol p2(fake2);
    show(fake1,fake2);
    p1.open(Route("/out","/in","text"));
    show(fake1,fake2);
    p2.open("/in");
    show(fake1,fake2);
    BufferedBlockWriter writer;
    writer.appendLine("d");
    writer.appendLine("0 \"Hello\"");
    p1.write(writer);
    show(fake1,fake2);
    BlockReader& reader = p2.beginRead();
    String str1 = reader.expectLine();
    String str2 = reader.expectLine();
    p2.endRead();
    //ACE_OS::printf("strings are [%s] [%s]\n",
    assertion(str1,String("d"));
    assertion(str2,String("0 \"Hello\""));

    // now we get funky
    ACE_OS::printf("Structured replies are possible\n");
    p2.write(writer);
    show(fake1,fake2);

    BlockReader& reader2 = p1.beginRead();
    String str0 = reader2.expectLine(); // deal with welcome line
    str1 = reader2.expectLine();
    str2 = reader2.expectLine();
    p1.endRead();
    ACE_OS::printf("strings are [%s] [%s]\n", str1.c_str(), str2.c_str());
    assertion(str1,String("d"));
    assertion(str2,String("0 \"Hello\""));

  } catch (IOException e) {
    ACE_OS::printf("exception FAILURE: %s\n", e.toString().c_str());
  }
}

static void checkTcpIO() {
  TcpFace face;
  face.open(Address("localhost",8000,"tcp"));
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

static void checkBottle() {
  Bottle bot;
  bot.addInt(5);
  bot.addString("hello \"my\" \\friend");
  String txt = bot.toString();
  ACE_OS::printf("bottle is [%s]\n", txt.c_str());
  assertion(txt,String("5 \"hello \\\"my\\\" \\\\friend\""));
  bot.fromString(bot.toString());
  ACE_OS::printf("bottle is [%s]\n", bot.toString().c_str());
  assertion(bot.toString(),String("5 \"hello \\\"my\\\" \\\\friend\""));
  bot.fromString("5 \"hello\"");
  ACE_OS::printf("bottle is [%s]\n", bot.toString().c_str());
  ACE_OS::printf("byte count is %d\n", bot.byteCount());
}

/**
 * This is a gateway for a test harness.
 * PENDING: This method should be moved into the yarp namespace.
 */
int yarp_test_main(int argc, char *argv[]) {
  if (argc<=1) {
    ACE_OS::printf("yarp testing underway\n");
    checkBottle();
    return 0;
    checkString();
    checkAddress();
    checkInputStreams();
    checkOutputStreams();
    checkTwoWayStreams();
    checkBlocks();
    //checkCarriers();
    //checkFakeFace();
    //checkTcpFace();
    checkProtocol();
    //checkTcpIO();
    checkCompanion(argc,argv);
    checkNameClient();
    ACE_OS::printf("yarp testing done\n");
  } else {
    return Companion::main(argc,argv);
  }
  return 0;
}

