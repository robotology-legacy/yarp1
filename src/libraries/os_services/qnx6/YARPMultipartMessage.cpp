#include <assert.h>
#include <sys/neutrino.h>
#include <unistd.h>

#include "YARPMultipartMessage.h"

class Block 
{
public:
  iov_t iov;
  
  Block() {}

  Block(char *buffer, int len)
    {
      Set(buffer,len);
    }

  void Set(char *buffer, int len)
    {
/* MIG4NTO           _setmx Replaced by SETIOV() for use with the QNX Neutrino */
/* MIG4NTO                  Msg*() functions. */
      //_setmx(this,buffer,len);
      SETIOV(&iov, buffer, len);
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
/* MIG4NTO            MK_FP NO LONGER SUPPORTED */
  //return (char *) MK_FP(blk->mxfer_seg,blk->mxfer_off);
  return ((char *) blk->iov.iov_base); //buh???
}


int YARPMultipartMessage::GetBufferLength(int index)
{
  assert(index<=top_index);
  Block *blk = ((Block*)system_resource) + index;
  return blk->iov.iov_len;
}

