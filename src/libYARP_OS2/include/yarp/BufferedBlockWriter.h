#ifndef _YARP2_BUFFEREDBLOCKWRITER_
#define _YARP2_BUFFEREDBLOCKWRITER_

#include <yarp/BlockWriter.h>
#include <yarp/SizedWriter.h>
#include <yarp/ManagedBytes.h>
#include <yarp/Logger.h>
#include <yarp/NetType.h>
#include <yarp/StringOutputStream.h>

#include <ace/Vector_T.h>

namespace yarp {
  class BufferedBlockWriter;
}

/**
 * A helper for creating cached object descriptions.
 */
class yarp::BufferedBlockWriter : public BlockWriter, public SizedWriter {
public:

  BufferedBlockWriter(bool textMode = false) : textMode(textMode) {
  }

  virtual ~BufferedBlockWriter() {
    clear();
  }

  void reset(bool textMode) {
    this->textMode = textMode;
    clear();
  }

  void clear() {
    for (unsigned int i=0; i<lst.size(); i++) {
      delete lst[i];
    }
    lst.clear();
  }

  virtual void appendBlock(const Bytes& data) {
    lst.push_back(new ManagedBytes(data,false));
  }

  virtual void appendBlockCopy(const Bytes& data) {
    ManagedBytes *buf = new ManagedBytes(data,false);
    buf->copy();
    lst.push_back(buf);
  }

  virtual void appendInt(int data) {
    NetType::NetInt32 i = data;
    Bytes b((char*)(&i),sizeof(i));
    ManagedBytes *buf = new ManagedBytes(b,false);
    buf->copy();
    lst.push_back(buf);
  }

  virtual void appendString(const String& data) {
    Bytes b((char*)(data.c_str()),data.length()+1);
    ManagedBytes *buf = new ManagedBytes(b,false);
    buf->copy();
    lst.push_back(buf);
  }

  virtual void appendBlock(const String& data) {
    Bytes b((char*)(data.c_str()),data.length()+1);
    ManagedBytes *buf = new ManagedBytes(b,false);
    lst.push_back(buf);
  }

  virtual void appendLine(const String& data) {
    String copy = data;
    copy += '\n';
    Bytes b((char*)(copy.c_str()),copy.length());
    ManagedBytes *buf = new ManagedBytes(b,false);
    buf->copy();

    //ACE_DEBUG((LM_DEBUG,"adding a line - %d bytes", copy.length()));

    lst.push_back(buf);
  }

  virtual bool isTextMode() {
    return textMode;
  }

  void write(OutputStream& os) {
    for (unsigned int i=0; i<lst.size(); i++) {
      ManagedBytes& b = *(lst[i]);
      //ACE_DEBUG((LM_DEBUG,"output a block, %d bytes",b.length()));
      os.write(b.bytes());
    }    
  }

  virtual int length() {
    return lst.size();
  }

  virtual int length(int index) {
    ManagedBytes& b = *(lst[index]);
    return b.length();
  }

  virtual const char *data(int index) {
    ManagedBytes& b = *(lst[index]);
    return (const char *)b.get();
  }


  String toString() {
    StringOutputStream sos;
    write(sos);
    return sos.toString();
  }

private:
  ACE_Vector<ManagedBytes *> lst;
  bool textMode;
};

#endif

