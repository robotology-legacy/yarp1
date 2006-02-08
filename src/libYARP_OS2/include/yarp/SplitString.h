#ifndef _YARP2_SPLITSTRING_
#define _YARP2_SPLITSTRING_

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_string.h>

namespace yarp {
  class SplitString;
}


#define MAX_ARG_CT (20)
#define MAX_ARG_LEN (256)


/**
 * Split a string into pieces.
 * This class is a bit crufty, and should be replaced.
 */
class yarp::SplitString {
private:
  int argc;
  const char *argv[MAX_ARG_CT];
  char buf[MAX_ARG_CT][MAX_ARG_LEN];

public:
  SplitString() {
    argc = 0;
  }

  SplitString(const char *command, const char splitter = ' ') {
    apply(command,splitter);
  }

  int size() {
    return argc;
  }

  void set(int index, const char *txt) {
    if (index>=0&&index<size()) {
      ACE_OS::strncpy(buf[index],(char*)txt,MAX_ARG_LEN);
    }
  }

  const char *get(int idx) {
    return buf[idx];
  }

  const char **get() {
    return (const char **)argv;
  }

  void apply(const char *command, char splitter=' ') {
    int at = 0;
    int sub_at = 0;
    unsigned int i;
    for (i=0; i<strlen(command)+1; i++) {
      if (at<MAX_ARG_CT) {
	char ch = command[i];
	if (ch>=32||ch=='\0'||ch=='\n') {
	  if (ch==splitter||ch=='\n') {
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

#endif

