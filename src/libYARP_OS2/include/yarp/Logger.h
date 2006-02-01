#ifndef _YARP2_LOGGER_
#define _YARP2_LOGGER_

#include <yarp/String.h>

#include <ace/Log_Msg.h>
#include <ace/Log_Record.h>
#include <ace/Log_Msg_Callback.h>

namespace yarp {
  class Logger;
}

/**
 *
 * This is currently a sad mixture of the jyarp logging mechanism
 * and ACE.  PENDING: make it less schizophrenic...
 *
 */
class yarp::Logger : public ACE_Log_Msg_Callback {
public:
  enum Level {
    DEBUG = LM_DEBUG,
    MAJOR_DEBUG = LM_INFO,
    WARNING = LM_WARNING,
    INFO = LM_INFO,
    SEVERE = LM_ERROR,

    DEFAULT_WARN=WARNING,
  };

  Logger(const char *prefix, Logger *parent = NULL) {
    this->prefix = prefix;
    this->parent = parent;
    verbose = 0;
    low = DEFAULT_WARN;
    if (this==&root) {
      ACE_Log_Msg *acer = ACE_Log_Msg::instance();
      acer->set_flags(8);
      acer->clr_flags(1);
      acer->msg_callback(this);
    }
  }

  Logger(const char *prefix, Logger& parent) {
    this->prefix = prefix;
    this->parent = &parent;
    low = DEFAULT_WARN;
  }

  static Logger& get();

  virtual void log(ACE_Log_Record& log_record) {
    show(log_record.type(),log_record.msg_data());
  }

  void debug(const char *txt) {
    show(DEBUG,txt);
  }

  void println(const char *txt) {
    debug(txt);
  }

  void info(const char *txt) {
    show(INFO,txt);
  }

  void warning(const char *txt) {
    show(WARNING,txt);
  }

  void error(const char *txt) {
    show(SEVERE,txt);
  }

  void fail(const char *txt) {
    show(SEVERE,txt);
    exit(1);
  }

  void assertion(bool cond) {
    if (!cond) {
      fail("assertion failure");
    }
  }

  void setVerbosity(int verbose = 0) {
    this->verbose = verbose;
  }

private:
  void show(int level, const char *txt);
  void exit(int result);

  static Logger root;
  yarp::String prefix;
  Logger *parent;
  int verbose;
  int low;
};

// compromise - use macros so that debugging can evaporate in optimized code.
// also, make a printf-style adaptor since c++ is a bit of a pain to 
// build strings in.
#define YARP_ERROR(log,x) log.error(x)
#define YARP_WARN(log,x)  log.warn(x)
#define YARP_INFO(log,x)  log.info(x)
#define YARP_DEBUG(log,x) log.debug(x)
#define YARP_FAIL(log,x)  log.fail(x)

#define YARP_ASSERT(x) if (!(x)) { ACE_OS::printf("Assertion failure %s:%d  !(%s)\n",__FILE__,__LINE__, #x ); ACE_OS::exit(1); }


#endif
