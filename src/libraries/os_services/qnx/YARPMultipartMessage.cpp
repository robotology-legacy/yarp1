#include <assert.h>
#ifdef __QNX4__
#include <sys/sendmx.h>
#endif


#include "YARPMultipartMessage.h"

class Block : public _mxfer_entry
{
public:
  Block() {}

  Block(char *buffer, int len)
    {
      Set(buffer,len);
    }

  void Set(char *buffer, int len)
    {
      _setmx(this,buffer,len);
    }
};

/* BEGIN GENERIC YARPMULTIPARTMESSAGE */

YARPMultipartMessage::~YARPMultipartMessage()
{
  if (owned)
    {
      if (system_resource!=NULL)
	{
	  delete[] ((Block*)system_resource);
	  system_resource = NULL;
	}
    }
}

void YARPMultipartMessage::Resize(int n_length)
{
  if (n_length>length)
    {
      if (owned)
	{
	  if (system_resource!=NULL)
	    {
	      delete[] ((Block*)system_resource);
	      system_resource = NULL;
	    }
	}
      system_resource = new Block[n_length];
      assert(system_resource!=NULL);
      length = n_length;
    }
  top_index = -1;
  owned = 1;
}

void YARPMultipartMessage::Set(int index, char *buffer, int buffer_length)
{
  assert(index<length);
  ((Block*)system_resource)[index].Set(buffer,buffer_length);
  if (index>top_index)
    {
      top_index = index;
    }
}

void YARPMultipartMessage::Reset()
{
  top_index = -1;
}


/* END GENERIC YARPMULTIPARTMESSAGE */

char *YARPMultipartMessage::GetBuffer(int index)
{
  assert(index<=top_index);
  Block *blk = ((Block*)system_resource) + index;
  return (char *) MK_FP(blk->mxfer_seg,blk->mxfer_off);
}


int YARPMultipartMessage::GetBufferLength(int index)
{
  assert(index<=top_index);
  Block *blk = ((Block*)system_resource) + index;
  return blk->mxfer_len;
}

