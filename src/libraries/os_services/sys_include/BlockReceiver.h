
#ifndef BLOCKRECEIVER_H_INC
#define BLOCKRECEIVER_H_INC

#include "YARPNameService.h"

#include "YARPPortContent.h"

class BlockReceiver : public YARPPortReader
{
public:
  YARPNameID pid;
  int has_msg;
  int offset;
  int reply_pending;
  int failed;

  BlockReceiver() { Begin(); }

  void Begin() { failed = 0; pid.Invalidate();  has_msg = 0; 
                 offset = 0; reply_pending = 0; }
  int Get();
  int Get(char *buffer, int len);
  //void UnGet(int delta);
  int End();

  virtual int Read(char *buffer, int length)
    {
      return Get(buffer,length);
    }

};



#endif
