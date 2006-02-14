#ifndef _YARP2_CONNECTIONWRITER_
#define _YARP2_CONNECTIONWRITER_

namespace yarp {
  namespace os {
    class ConnectionWriter;
  }
}

class yarp::os::ConnectionWriter {
public:

  virtual ~ConnectionWriter() {}

  /**
   * Makes a copy of your data for the message
   */
  virtual void appendBlock(const char *data, int len) = 0;

  /**
   * Makes a copy of your data for the message
   */
  virtual void appendInt(int data) = 0;

  /**
   * Makes a copy of your data for the message
   */
  virtual void appendString(const char *str, int terminate = '\n') = 0;

  /**
   * no copy made - make sure this block remains untouched
   * If you want to know when it can be touched again, 
   * register a callback.
   */
  virtual void appendExternalBlock(const char *data, int len) = 0;

  virtual bool isTextMode() = 0;

  /**
   * If you can easily determine how many blocks there are in a message, 
   * call this first, before anything else.  
   * May improve efficiency in some situations, someday.
   */
  virtual void declareSizes(int argc, int *argv) = 0;

  /**
   * If you placed want to know when everything is sent,
   * you can request a callback here.
   */
  //virtual void registerCallback(PortSendListener& listener,
  //			void *key) = 0;
};

#endif
