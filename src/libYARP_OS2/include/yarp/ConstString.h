#ifndef _YARP2_CONSTSTRING_
#define _YARP2_CONSTSTRING_

namespace yarp {
  namespace os {
    class ConstString;
  }
}

/**
 * YARP library returns strings in this form.
 */
class yarp::os::ConstString {
public:
  ConstString();
  ConstString(const char *str);
  ~ConstString();
  ConstString(const ConstString& alt);
  
  const ConstString& operator = (const ConstString& alt);

  const char *c_str() const;
private:
  void *implementation;
};

#endif

