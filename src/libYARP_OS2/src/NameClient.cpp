#include <yarp/NameClient.h>
#include <yarp/Logger.h>
#include <yarp/TcpFace.h>
#include <yarp/NetType.h>


using namespace yarp;

NameClient NameClient::instance;


/*
  Old class for splitting string based on spaces
 */

#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)

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


String NameClient::send(const String& cmd) {
  ACE_DEBUG((LM_DEBUG,">>> sending to nameserver: %s",cmd.c_str()));
  String result;
  TcpFace face;
  OutputProtocol *ip = face.write(getAddress());
  String cmdn = cmd + "\n";
  Bytes b((char*)cmdn.c_str(),cmdn.length());
  ip->getOutputStream().write(b);
  bool more = true;
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
  ACE_DEBUG((LM_DEBUG,"<<< received from nameserver: %s",result.c_str()));
  return result;
}


