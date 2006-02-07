#include <yarp/NameClient.h>
#include <yarp/Logger.h>
#include <yarp/TcpFace.h>
#include <yarp/NetType.h>
#include <yarp/NameServer.h>
#include <yarp/NameConfig.h>
#include <yarp/FallbackNameClient.h>

using namespace yarp;

/**
 * The NameClient singleton
 */


NameClient *NameClient::instance = NULL;



/*
  Old class for splitting string based on spaces
 */

#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)


#ifndef DOXYGEN_SHOULD_SKIP_THIS

class Params {
private:
  int argc;
  const char *argv[MAX_ARG_CT];
  char buf[MAX_ARG_CT][MAX_ARG_LEN];

public:
  Params() {
    argc = 0;
  }

  Params(const char *command) {
    apply(command);
  }

  int size() {
    return argc;
  }

  const char *get(int idx) {
    return buf[idx];
  }

  void apply(const char *command) {
    int at = 0;
    int sub_at = 0;
    unsigned int i;
    for (i=0; i<strlen(command)+1; i++) {
      if (at<MAX_ARG_CT) {
	char ch = command[i];
	if (ch>=32||ch=='\0'||ch=='\n') {
	  if (ch==' '||ch=='\n') {
	    ch = '\0';
	  }
	  if (sub_at<MAX_ARG_LEN) {
	    buf[at][sub_at] = ch;
	    sub_at++;
	  }
	}
	if (ch == '\0') {
	  if (sub_at>1) {
	    at++;
	  }
	  sub_at = 0;
	} 
      }
    }
    for (i=0; i<MAX_ARG_CT; i++) {
      argv[i] = buf[i];
      buf[i][MAX_ARG_LEN-1] = '\0';
    }

    argc = at;
  }
};

#endif /*DOXYGEN_SHOULD_SKIP_THIS*/





Address NameClient::extractAddress(const String& txt) {
  Address result;
  Params p(txt.c_str());
  if (p.size()>=9) {
    // registration name /bozo ip 5.255.112.225 port 10002 type tcp
    const char *regName = p.get(2);
    const char *ip = p.get(4);
    int port = atoi(p.get(6));
    const char *carrier = p.get(8);
    result = Address(ip,port,carrier,regName);
  }
  return result;
}


String NameClient::send(const String& cmd, bool multi) {
  ACE_DEBUG((LM_DEBUG,">>> sending to nameserver: %s",cmd.c_str()));
  String result;
  TcpFace face;
  OutputProtocol *ip = face.write(getAddress());
  if (ip==NULL) {
    YARP_INFO(Logger::get(),"no connection to nameserver, scanning mcast");
    Address alt = FallbackNameClient::seek();
    if (alt.isValid()) {
      NameConfig nc;
      nc.setAddress(alt);
      nc.toFile();
      address = alt;
      ip = face.write(getAddress());
      if (ip==NULL) {
	YARP_ERROR(Logger::get(),
		   "no connection to nameserver, scanning mcast");
	return "";
      }
    } else {
      return "";
    }
  }
  String cmdn = cmd + "\n";
  Bytes b((char*)cmdn.c_str(),cmdn.length());
  ip->getOutputStream().write(b);
  bool more = multi;
  while (more) {
    String line = NetType::readLine(ip->getInputStream());
    if (line.length()>1) {
      if (line[0] == '*') {
	more = false;
      }
    }
    result += line + "\n";
  }
  ip->close();
  delete ip;
  ACE_DEBUG((LM_DEBUG,"<<< received from nameserver: %s",result.c_str()));
  return result;
}



Address NameClient::queryName(const String& name) {
  String np = getNamePart(name);
  if (isFakeMode()) {
    return getServer().queryName(np);
  }
  String q("NAME_SERVER query ");
  q += np;
  return probe(q);
}

Address NameClient::registerName(const String& name) {
  return registerName(name,Address());
}

Address NameClient::registerName(const String& name, const Address& suggest) {
  String np = getNamePart(name);
  if (isFakeMode()) {
    return getServer().registerName(np,suggest);
  }
  String q("NAME_SERVER register ");
  q += np;
  if (suggest.isValid()) {
    q += " ";
    q += suggest.getCarrierName();
    q += " ";
    q += suggest.getName();
    q += " ";
    if (suggest.getPort()==0) {
      q += "...";
    } else {
      q += NetType::toString(suggest.getPort());
    }
  }
  Address address = probe(q);
  if (address.isValid()) {
    String reg = address.getRegName();
    send(String("NAME_SERVER set ") + reg + " offers tcp text udp",false);
    send(String("NAME_SERVER set ") + reg + " accepts tcp text udp",false);
  }
  return address;
}

Address NameClient::unregisterName(const String& name) {
  String np = getNamePart(name);
  if (isFakeMode()) {
    return getServer().unregisterName(np);
  }
  String q("NAME_SERVER unregister ");
  q += np;
  return probe(q);
}


NameClient::~NameClient() {
  if (fakeServer!=NULL) {
    delete fakeServer;
    fakeServer = NULL;
  }
}

NameServer& NameClient::getServer() {
  if (fakeServer==NULL) {
    fakeServer = new NameServer;
  }
  YARP_ASSERT(fakeServer!=NULL);
  return *fakeServer;
}



NameClient::NameClient() {
  NameConfig conf;
  address = Address();
  if (conf.fromFile()) {
    address = conf.getAddress();
  } else {
    YARP_ERROR(Logger::get(),"Cannot find name server");
    //address = Address("localhost",10000);
  }
  YARP_DEBUG(Logger::get(),String("name server address is ") + 
	     address.toString());
  fake = false;
  fakeServer = NULL;
}
