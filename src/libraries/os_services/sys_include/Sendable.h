
#ifndef SENDABLE_H_INC
#define SENDABLE_H_INC

#include <string.h>

//#include "debug-new.h"

#include "BlockSender.h"
#include "BlockReceiver.h"
#include "RefCounted.h"

class Sendables;

class Sendable : public RefCounted
{
public:
  Sendables *owner;

  Sendable() { owner = NULL; }
  
  virtual ~Sendable() {}

  virtual int Write(BlockSender& sender) { return 1; }
  virtual int Read(BlockReceiver& receiver) { return 1; }

  virtual int Destroy();
};

#define Receivable Sendable
#define Gossip Sendable

template <class T>
class SimpleSendable : public Sendable
{
public:
  T datum;

  T& Content()
    { return datum; }
  
  virtual int Write(BlockSender& sender)
    {
      sender.Add((char*)(&datum),sizeof(datum));
      return 1;
    }
  virtual int Read(BlockReceiver& receiver)
    {
      receiver.Get((char*)(&datum),sizeof(datum));
      return 1;
    }
};


#define MAX_SIMPLE_TEXT 1000

typedef char SimpleTextMsg[MAX_SIMPLE_TEXT];

class TextSendable : public SimpleSendable<SimpleTextMsg>
{
public:
  virtual int Write(BlockSender& sender)
    {
      sender.Add((char*)(Content()),1+strlen(Content()));
      return 1;
    }
};


/*
class ImageSendable : public Sendable
{
public:
  struct Header
  {
    int len;
    int w;
    int h;
    int depth;
  } header;
  CountedPtr<Buffer> buffer;

  ImageSendable(Buffer *n_buffer, int h, int w, int depth)
    {
      buffer.Set(n_buffer);
      header.h = h;
      header.w = w;
      header.depth = depth;
      header.len = h*w*depth;
    }

  virtual ~ImageSendable()
    {
    }

  virtual int Write(BlockSender& sender)
    {
      //sender.Add((char*)(&header),sizeof(header));  // HIT Tag output
      assert(buffer.Ptr()!=NULL);
      char *mem = buffer.Ptr()->memory;
      assert(mem!=NULL);
      sender.Add(mem,header.len);
      return 1;
    }


  virtual int Read(BlockReceiver& receiver)
    {
      assert(0==1);
      return 1;
    }
};
*/



#endif
