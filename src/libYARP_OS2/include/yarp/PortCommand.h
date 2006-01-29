#ifndef _YARP2_PORTCOMMAND_
#define _YARP2_PORTCOMMAND_

#include <yarp/Portable.h>
#include <yarp/ManagedBytes.h>

namespace yarp {
  class PortCommand;
}

class yarp::PortCommand : public Portable {
public:

  PortCommand() : header(8) {
    ch = '\0';
    str = "";
  }

  PortCommand(char ch, const String& str) : header(8) {
    this->ch = ch;
    this->str = str;
  }

  virtual void readBlock(BlockReader& reader);

  virtual void writeBlock(BlockWriter& writer);

  char getKey() {
    return ch;
  }

  String getText() {
    return str;
  }

public:
  char ch;
  String str;
  ManagedBytes header;
};

#endif
