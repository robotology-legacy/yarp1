
#include <yarp/YARPTime.h>
#include <yarp/YARPNameID_defs.h>
#include <yarp/YARPNameClient2.h>

#define YNC if(1) printf
//#define YNC if(0) printf

#define DBG if (0)
//#define DBG if (1)

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
    int i;
    for (i=0; i<strlen(command)+1; i++) {
      if (at<MAX_ARG_CT) {
	char ch = command[i];
	if (ch>=32||ch=='\0') {
	  if (ch==' ') {
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




YARPNameClient2::YARPNameClient2() {
  active = 0;
  at = 0;
  len = 0;
}


int YARPNameClient2::registerName(const char *name,
				  const char *ip, const char *type,
				  ACE_INET_Addr& addr) {
  YNC("registerName %s\n", name);
  YARPString cmd("NAME_SERVER register ");
  cmd = cmd + name + " tcp ... ... 10\n";
  YARPString result = send(cmd,true);
  Params p(result.c_str());
  if (p.size()>=9) {
    // registration name /bozo ip 5.255.112.225 port 10002 type tcp
    const char *ip = p.get(4);
    int port = atoi(p.get(6));
    YNC("registration is for ip %s and port %d\n", ip, port);
    //addr.set(ip);
    addr.set(port,ip);
    YNC("confirm registration is for ip %s and port %d\n", 
	addr.get_host_addr(), addr.get_port_number());
    send(YARPString("NAME_SERVER set ") + name + " offers tcp udp mcast shmem\n");
    send(YARPString("NAME_SERVER set ") + name + " accepts tcp udp mcast shmem\n");
    return YARP_OK;
  }
  return YARP_FAIL;
}

int YARPNameClient2::queryName(const char *name, ACE_INET_Addr& addr, 
			       int *type) {
  YNC("queryName %s\n", name);
  YARPString cmd("NAME_SERVER query ");
  name = "/read";
  cmd = cmd + name + "\n";
  YARPString result = send(cmd,true);
  Params p(result.c_str());
  if (p.size()>=9) {
    // registration name /bozo ip 5.255.112.225 port 10002 type tcp
    const char *ip = p.get(4);
    int port = atoi(p.get(6));
    addr.set(port,ip);
    //addr.set_port_number(port);
    if (type!=NULL) {
      int tnum = 0;
      switch (p.get(8)[0]) {
      case 'u':
	tnum = YARP_UDP;
	break;
      case 's':
	tnum = YARP_SHMEM;
	break;
      case 'm':
	tnum = YARP_MCAST;
	break;
      case 't':
	tnum = YARP_TCP;
	break;
      default:
	tnum = -1;
	break;
      }
      *type = tnum;
      YNC("registration is for ip %s and port %d, type %d\n", ip, port, tnum);
    }
    return YARP_OK;
  }
  return YARP_FAIL;
}

int YARPNameClient2::check(const char *name, const char *key, 
			   const char *value) {
  YNC("check %s\n", name);
  YARPString cmd("NAME_SERVER check ");
  cmd = cmd + name + " ";
  cmd = cmd + key + " ";
  cmd = cmd + value + "\n";
  YARPString result = send(cmd,false);
  Params p(result.c_str());
  if (p.size()>=8) {
    // port /write property offers value mcast present true
    int present = p.get(7)[0]=='t';
    YNC("check result is %d\n", present);
    return present;
  }
  return 0;
}



YARPString YARPNameClient2::send(const YARPString& cmd, int multiline) {
  at = 0;
  len = 0;
  YARPString result = "";
  ACE_SOCK_Connector connector_;
  ACE_SOCK_Stream client_stream_;
  if (connector_.connect (client_stream_, server) == -1) {
      ACE_DEBUG ((LM_DEBUG, "(%P|%t) %p\n","connection failed"));
      return result;
    }
  char *str = (char*)cmd.c_str();
  iovec iov[1];
  iov[0].iov_base = str;
  iov[0].iov_len = strlen(str);

  YNC("sending text %s", str);
  int sent = client_stream_.sendv_n (iov, 1);
  
  //  if (sent == -1)
    //    ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p\n","send_n"),0);
  if (sent!=-1) {
    int done = 1;
    result = receive(client_stream_);
    YNC("received %s\n", result.c_str());
    YARPString more = "x";
    while (multiline && more.c_str()[0]!='*') {
      // ignore rest
      more = receive(client_stream_);
      YNC("received %s\n", more.c_str());
    }
  } else {
    YNC("problem with connection");
  }
  client_stream_.close();

  return result;
}


const char *YARPNameClient2::receive(ACE_SOCK_Stream& is) {
  double timeout = 0;
  DBG printf("*** at %d len %d\n", at, len);
  if (len>0) {
    if (at!=0) {
      memmove(current,current+at,len);
      at = 0;
    }
  }
  bool has_more = false;
  int add = 0;
  int sub_add = 0;
  while (!has_more && add>=0) {
    DBG printf("bing\n");
    for (int i=0; i<MAX_TELNET_BUFFER&&i<len; i++) {
      if (current[i]=='\x0d'||current[i]=='\x0a') {
	has_more = true;
	break;
      }
    }
    int prev_len = len;
    if (!has_more) {
      add = receive(is,current+len,MAX_TELNET_BUFFER-len,timeout);
      if (add>=0) {
	len += add;
      }
    }

    DBG { 
      printf("[*** got %d]\n", add);
      if (add>=0) {
	for (int i=0; i<add;i++) {
	  char *stuff = current+prev_len;
	  if (stuff[i]>=32) {
	    printf("[0x%0x %d '%c']\n", stuff[i], stuff[i], stuff[i]);
	  } else {
	    printf("[0x%0x %d]\n", stuff[i], stuff[i]);
	  }
	}
      }
    }

    if (add==0) {
      has_more = true;
    }
  }

  if (add>=0) {
    bool found = false;
    int i;
    for (i=0; i<MAX_TELNET_BUFFER&&i<len; i++) {
      if (current[i]=='\x0d' || current[i]=='\x0a') {
	if (current[i]=='\x0a') {
	  found = true;
	}
	current[i] = '\0';
	if (found) {
	  break;
	}
      }
    }
    if (found) {
      len = len-i-1;
      if (len>0) {
	at = i+1;
      } else {
	at = 0;
	len = 0;
      }
      return current;
    }
  }
  return NULL;
}


int YARPNameClient2::receive(ACE_SOCK_Stream& is, char *data, int len, 
			     double timeout) {
  /*
  size_t byte_count = 0;
  iovec iov[1];
  iov[0].iov_len = len;
  iov[0].iov_base = data;
  */
  //int res = is.recvv_n(iov, 1, 0, &byte_count);  
  int res = is.recv(data,len);
  //if (res>=0) {
  //return byte_count;
  //  }
  /*
  if (res>0) {
    for (int i=res-1; i>=0; i--) {
      if (data[i]=='\0') {
	res--;
      }
    }
  }
  */
  return res;
}


