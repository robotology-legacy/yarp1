#include <assert.h>
#include <stdio.h>
#include "YARPNativeSyncComm.h"
#include "YARPNameID_defs.h"

#include <sys/kernel.h>

#include "YARPThread.h"

// deal with inconvenient macros
#ifdef Send
#undef Send
#undef Receive
#undef Reply
#endif

class TSData
{
public:
  int offset;
};

// only one reader per thread, a la raw QNX protocol, so can use thread-global
// variable
YARPThreadSpecific<TSData> ts_data;


int YARPNativeSyncComm::Send(YARPNameID dest, char *buffer, 
				 int buffer_length,
				 char *return_buffer, int return_buffer_length)
{
  assert(dest.IsConsistent(YARP_NAME_MODE_NATIVE));
  return __send(dest.GetRawIdentifier(),buffer,return_buffer,
		buffer_length,return_buffer_length);
}


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, char *buffer, 
				    int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  //  printf("^^^^ blocking receive for id %d\n", src.GetRawIdentifier());
  int result =  __receive(src.GetRawIdentifier(),buffer,buffer_length);
  ts_data.Content().offset = buffer_length; //no way to know real transfer :(
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, char *buffer, 
				   int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  int result = ::Creceive(src.GetRawIdentifier(),buffer,buffer_length);
  ts_data.Content().offset = buffer_length; //no way to know real transfer :(
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}

int YARPNativeSyncComm::ContinuedReceive(YARPNameID src, char *buffer, 
					 int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  int result = -1;
  if (src.IsValid())
    {
      int offset = ts_data.Content().offset;
      int more = Readmsg(src.GetRawIdentifier(),offset,buffer,buffer_length);
      if (more>=0)
	{
	  offset += more;
	  ts_data.Content().offset = offset;
	  result = more;
	}
    }
  return result;
}

int YARPNativeSyncComm::Reply(YARPNameID src, char *buffer, 
				  int buffer_length)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  return __reply(src.GetRawIdentifier(),buffer,buffer_length);
}


int YARPNativeSyncComm::Send(YARPNameID dest, YARPMultipartMessage& msg,
				 YARPMultipartMessage& return_msg)
{
  assert(dest.IsConsistent(YARP_NAME_MODE_NATIVE));
  return Sendmx(dest.GetRawIdentifier(), msg.GetParts(), return_msg.GetParts(),
		(struct _mxfer_entry*) msg.GetRawBuffer(),
		(struct _mxfer_entry*) return_msg.GetRawBuffer());
}


YARPNameID YARPNativeSyncComm::BlockingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  int result =  Receivemx(src.GetRawIdentifier(), msg.GetParts(), 
			  (struct _mxfer_entry*) msg.GetRawBuffer());
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


YARPNameID YARPNativeSyncComm::PollingReceive(YARPNameID src, YARPMultipartMessage& msg)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  int result = Creceivemx(src.GetRawIdentifier(), msg.GetParts(), 
			  (struct _mxfer_entry*) msg.GetRawBuffer());
  return YARPNameID(YARP_NAME_MODE_NATIVE,result);
}


int YARPNativeSyncComm::Reply(YARPNameID src, YARPMultipartMessage& msg)
{
  assert(src.IsConsistent(YARP_NAME_MODE_NATIVE));
  return Replymx(src.GetRawIdentifier(), msg.GetParts(), 
		 (struct _mxfer_entry*) msg.GetRawBuffer());
}



