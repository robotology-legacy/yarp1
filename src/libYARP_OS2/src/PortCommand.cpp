#include <yarp/PortCommand.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>

using namespace yarp;

void PortCommand::readBlock(BlockReader& reader) {
  //ACE_DEBUG((LM_DEBUG,"PortCommand::readBlock"));
  ch = '\0';
  str = "";
  if (!reader.isTextMode()) {
    reader.expectBlock(header.bytes());
    char *base = header.get();
    if (base[4] == '~') {
      ch = base[5];
      if (ch=='\0') {
	str = reader.expectString(reader.getSize());
	if (str.length()>0) {
	  ch = str[0];
	}
      }
    } else {
      throw IOException("bad header in PortCommand::readBlock");
    }
  } else {
    //ACE_OS::printf("PortCommand::readBlock pre read\n");
    str = reader.expectLine();
    //ACE_OS::printf("PortCommand::readBlock post read\n");
    if (str.length()>0) {
      ch = str[0];
    }
  }
}

void PortCommand::writeBlock(BlockWriter& writer) {
  ACE_DEBUG((LM_DEBUG,"PortCommand::writeBlock"));
  //ACE_OS::printf("Writing port command, text mode %d\n", writer.isTextMode());
  if (!writer.isTextMode()) {
    int len = 0;
    if (ch=='\0') {
      len = str.length()+1;
    }
    YARP_ASSERT(header.length()==8);
    char *base = header.get();
    Bytes b(base,4);
    NetType::netInt(len,b);
    base[4] = '~';
    base[5] = ch;
    base[6] = 0;
    base[7] = 1;
    writer.appendBlock(header.bytes());
    if (ch=='\0') {
      writer.appendBlock(str);
    }
  } else {
    if (ch!='\0') {
      writer.appendLine(String(ch));
    } else {
      writer.appendLine(str);
    }
  }
}

  
