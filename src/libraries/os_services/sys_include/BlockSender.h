
#ifndef BLOCKSENDER_H_INC
#define BLOCKSENDER_H_INC

#include <vector>
#ifdef WIN32
#include <iostream>
#endif

#include "RefCounted.h"

#include "YARPNameID.h"
#include "YARPNameID_defs.h"

#include "YARPPortContent.h"


extern int __debug_level;

// Should find out exactly what this is
#define MAX_PACKET (128*128*3+100)

#ifdef __QNX__
#include <sys/sendmx.h>
#else
using namespace std;
/* must match Block class in YARPMultipartMessage implementation */
class _mxfer_entry
{
public:
  void *buffer;
  int len;
  
  void _set(void *n_buffer, int n_len)
  { buffer = n_buffer;  len = n_len; }
};
#define _setmx(p,b,l) (p)->_set((b),(l))
#endif


class BlockUnit : public _mxfer_entry
{
public:
  BlockUnit() {}

  BlockUnit(char *buffer, int len)
    {
      Set(buffer,len);
    }

  void Set(char *buffer, int len)
    {
      _setmx(this,buffer,len);
    }
};

class BlockSender : public YARPPortWriter
{
protected:
  int max_packet;

  int AddPiece(char *buffer, int len);

public:
  YARPNameID pid;
  int available;
  int pieces;
  int failed;
  vector<BlockUnit> entries;
  vector<BlockUnit>::iterator cursor;

  BlockSender()
    {
      max_packet = MAX_PACKET;
      Begin(YARPNameID(YARP_NAME_MODE_NULL,-1));
    }

  void SetMaxPacket(int n_max_packet)
    {
      available += (n_max_packet-max_packet);
      max_packet = n_max_packet;
    }

  void Begin(YARPNameID npid)
    {
      pid = npid;
      cursor = entries.begin();
      available = max_packet;
      pieces = 0;
      failed = 0;
    }

  int Add(char *buffer, int len);

  virtual int Write(char *buffer, int length)
    {
      return Add(buffer,length);
    }

  int End()
    {
      Fire();
      pid = YARPNameID(YARP_NAME_MODE_NULL,-1);
      return !failed;
    }

  int Fire();
};


template <class T>
class HeaderedBlockSender : public BlockSender
{
public:
  int add_header;
  vector<T> headers;
  vector<T>::iterator header_cursor;

  HeaderedBlockSender()
    {
      add_header = 0;
      header_cursor = headers.begin();
    }

  void Begin(YARPNameID pid)
    {
      add_header = 0;
      header_cursor = headers.begin();
      BlockSender::Begin(pid);
    }

  int Add(char *buffer, int len)
    {
      Check();
      return BlockSender::Add(buffer,len);
    }
  
  virtual int Write(char *buffer, int length)
    {
      return Add(buffer,length);
    }

  int End()
    {
      Check();
      return BlockSender::End();
    }

  void Check()
    {
      if (add_header)
	{
	  BlockSender::Add((char*)(header_cursor),sizeof(T));
	  add_header = 0;
	  header_cursor++;
	}
    }
  
  T *AddHeader()
    {
      Check();
      if (header_cursor == headers.end())
	{
	  if (__debug_level>=50)
	    {
	      //cout << "*** NEW stl " << __FILE__ << ":" << __LINE__ << endl;
	    }
	  header_cursor = headers.insert(header_cursor,T());
	}
      add_header = 1;
      return header_cursor;
    }
};


#endif
