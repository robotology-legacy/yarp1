#ifndef _YARP2_BYTES_
#define _YARP2_BYTES_


namespace yarp {
  class Bytes;
}


class yarp::Bytes {
public:
  Bytes() {
    data = 0;
    len = 0;
  }

  Bytes(char *data, int len) {
    this->data = data;
    this->len = len;
  }

  int length() const {
    return len;
  }

  char *get() const {
    return data;
  }

private:
  char *data;
  int len;
};

#endif

